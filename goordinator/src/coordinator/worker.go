package main

import (
  "../common"
  "encoding/binary"
  "log"
)

type WorkerChannels struct {
  addworker chan *Worker
  joinpool chan *Worker
  healthcheckRequest chan WCInfo
  nextID chan uint32
  gettaskRequest chan WCInfo
  taskresult chan common.Socket
  rmworker chan *Worker
}

// worker connection info
type WCInfo struct {
  ID uint32
  sock *common.Socket
}

type Worker struct {
  index int
  // buffered channel (buffer size is capacity)
  tasks chan *common.Task
  getResults chan bool
  // buffered channel operates with common.WorkerInfo
  ccinfo chan chan interface{}
  updatePending chan int64
  activeTasks map[int64]*common.Task
  // if positive means number of pending tasks 
  // else means number of task to retrieve from worker
  // used only from coordinator's select
  pending int64
  // used only for healthcheck
  cachedPending int64
  capacity int64
  tasksDone int64
  ID uint32
  getresultsFlag bool
}

func (w *Worker) IncPendingTasks() {
  w.pending++
  w.updatePending <- w.pending
}

func (w *Worker) GetStatus() interface{} { return w.tasksDone }
func (w *Worker) GetStatusChannel() chan chan interface{} { return w.ccinfo }

func (w *Worker) SetHealthStatus(tasksDone int64) { w.tasksDone = tasksDone }

func (w *Worker) GetHealthReply() interface{} {
  var result int32
  if !w.getresultsFlag {
    result = int32(w.cachedPending)
  } else {
    result = -1
  }
  
  return result
}

func (w *Worker) SetHealthReply(pending int64) {
  w.cachedPending = pending
}

func (w *Worker) GetID() uint32 { return w.ID }

func (w *Worker) GetResultsFlagChannel() chan bool { return w.getResults }
func (w *Worker) SetGetResultsFlag() { w.getresultsFlag = true }

func (w *Worker) GetUpdateReplyChannel() chan int64 { return w.updatePending }

type Pool []*Worker

func (p Pool) Len() int { return len(p) }

func (p Pool) Less(i, j int) bool {
  return p[i].pending < p[j].pending
}

func (p *Pool) Swap(i, j int) {
  a := *p
  a[i], a[j] = a[j], a[i]
  a[i].index = i
  a[j].index = j
}

func (p *Pool) Push(x interface{}) {
  a := *p
  n := len(a)
  a = a[0:n+1]
  w := x.(*Worker)
  a[n] = w
  w.index = n
  *p = a
}

func (p *Pool) Pop() interface{} {
  a := *p
  *p = a[0 : len(a) - 1]
  w := a[len(a) - 1]
  w.index = -1
  return w
}

func (w *Worker) RetrieveStatus(cinfo chan interface{}) {
  w.ccinfo <- cinfo
}

func (w *Worker) sendNextTask(sock *common.Socket) error {
  defer sock.Close()
  
  task := <- w.tasks

  _, ok := w.activeTasks[task.ID]
  if !ok {
    w.activeTasks[task.ID] = task
  } else {
    log.Fatalf("Task with ID %v is already sent to this worker", task.ID)
  }

  err := binary.Write(sock, binary.BigEndian, task.ID)
  
  if err == nil {
    var n int64
    n, err = common.WriteDataArray(sock, task.Parameters)
    log.Printf("Sent %v bytes of task %v data to worker #%v", n, task.ID, w.ID)
  }

  return err
}
