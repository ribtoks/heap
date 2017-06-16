package main

import (
  "../common"
  "log"
  "time"
  "encoding/binary"
  "container/heap"
)

type Coordinator struct {
  pool Pool
  client *Client
  hash map[uint32]*Worker
  lastWorkerID uint32
  // buffered
  tasks chan *common.Task
  initWorkerQueue chan *Worker
  joinPoolQueue chan *Worker
  collectResults chan bool
  doneTasksStat chan int64
  workerTimeout chan HealthReporter
  clientTimeout chan HealthReporter
  workerQuit chan bool
  clientQuit chan bool
}

func (c *Coordinator)handleWorkerChannels(wch WorkerChannels) {
  log.Println("Coordinator: worker channels handling started")
  defer log.Println("Coordinator: worker channels handling finished")

  statUpdateTicker := time.NewTicker(3*time.Second)
  
WorkerLoop:
  for {
    select {
    case w := <- wch.addworker: c.addWorker(w, wch.nextID)
    case w := <- c.joinPoolQueue: c.joinPool(w)
    case wci := <- wch.healthcheckRequest: c.checkHealthWorker(wci)
    case newTask := <- c.tasks: c.dispatch(newTask)
    case wci := <- wch.gettaskRequest: c.sendNextTaskToWorker(wci)
    case <- c.collectResults: c.setGetResultsFlag()
    case <- statUpdateTicker.C: c.refreshDoneStats()
    case hr := <- c.workerTimeout: c.workerTimeoutOccured(hr)
    case <- c.workerQuit: {
      break WorkerLoop
      }
    }
  }
}

func (c *Coordinator)handleClientChannels(cch ClientChannels) {
  log.Println("Coordinator: client channels handling started")
  defer log.Println("Coordinator: client channels handling finished")
  
ClientLoop:
  for {
    select {
    case sock := <- cch.addclient: c.addClient(sock)
    case sock := <- cch.healthcheckRequest: c.checkHealthClient(sock)
    case sock := <- cch.readcommondata: c.readCommonData(sock)
    case w := <- c.initWorkerQueue: c.initWorker(w)
    case stat := <- c.doneTasksStat: c.updateClientDoneStats(stat)
    case sock := <- cch.runcomputation: c.runComputation(sock)    
    case <- c.clientTimeout: {
      // TODO: cleanup
    }
    case <- c.clientQuit: {
      break ClientLoop
      }
    }
  }
}

func (c *Coordinator)quit() {
  log.Println("Coordinator: posting quit signal to worker and client quit channels")
  c.workerQuit <- true
  c.clientQuit <- true
}

func (c *Coordinator) addWorker(w *Worker, nextIDChan chan <- uint32) {
  nextID := c.getNextWorkerID()

  w.ID = nextID
  c.hash[nextID] = w

  nextIDChan <- nextID
  c.initWorkerQueue <- w

  log.Printf("Coordinator: added worker with ID #%v", nextID)
}

func (c *Coordinator) joinPool(w *Worker) {
  heap.Push(&c.pool, w)
  log.Printf("Coordinator: worker with id #%v joined pool", w.ID)
}

func (c *Coordinator) initWorker(w *Worker) {
  if c.client != nil && c.client.commondata != nil {
    log.Printf("Initializing connected worker with id #%v", w.ID)

    w.tasks <- &common.Task{0, *c.client.commondata}
    w.IncPendingTasks()
    log.Printf("Added common task to worker with id #%v", w.ID)

    log.Printf("Pushing worker #%v to join pool queue..", w.ID)
    c.joinPoolQueue <- w
  } else {
    // try to init same worker later
    go func(queue chan *Worker, worker *Worker) {time.Sleep(1*time.Second); queue <- worker} (c.initWorkerQueue, w)
  }
}

func (c *Coordinator) getNextWorkerID() (nextID uint32) {
  nextID = c.lastWorkerID
  c.lastWorkerID++
  return nextID
}

func (c *Coordinator) addClient(sock common.Socket) {
  client := Client{
    status: common.CIdle,
    info: make(chan chan interface{}),
    updateTasksDone: make(chan int64),
    ID: 0}

  defer sock.Close()

  log.Printf("Coordinator: add client with remote address %v \n", sock.RemoteAddr())
  
  canAddClient := c.client == nil
  if canAddClient {
    c.client = &client
  }
  
  client.replyInit(sock, canAddClient)
}

func (c *Coordinator) checkHealthWorker(wci WCInfo) {
  // TODO: add assert value,present = hash[addr]
  addr := wci.sock.RemoteAddr()

  log.Printf("Worker #%v healthcheck request from address: %v\n", wci.ID, addr)

  w, present := c.hash[wci.ID]
  if !present {
    log.Fatalf("Healthcheck: worker with address %v is not registered", addr)
  }

  go checkHealth(w, wci.sock, c.workerTimeout)
}

func (c *Coordinator) checkHealthClient(sock common.Socket) {
  addr := sock.RemoteAddr()

  log.Printf("Client healthcheck request from address: %v\n", addr)

  cl, present := c.client, c.client != nil
  if !present {
    log.Fatalf("Healthcheck: client with address %v is not registered", addr)
  }

  go checkHealth(cl, &sock, c.clientTimeout)
}

func (c *Coordinator) readCommonData(sock common.Socket) {
  defer sock.Close()
  log.Println("Reading common data from client")
  defer log.Println("Common data has been read")

  parameters, _, err := common.ReadDataArray(sock)
  if err != nil {
    log.Printf("Error while reading common data (%v)\n", err)
  }

  c.client.commondata = &parameters
}

func (c *Coordinator) runComputation(sock common.Socket) {
  defer sock.Close()
  
  log.Println("Coordinator: reading specific parameters")

  // read all tasks parameters and create tasks
  var tcount, i uint32
  err := binary.Read(sock, binary.BigEndian, &tcount)
  // TODO: handle error
  if err != nil {
    log.Fatal("Fatal Error while running computation(%v)\n", err)
  }

  log.Printf("Coordinator: going to receive %v tasks\n", tcount)

  taskID := int64(1)

  for i = 0; i < tcount; i++ {
    parameters, n, err := common.ReadDataArray(sock)

    if err == nil {
      log.Printf("Read task with id #%v and data size [%v]", taskID, n)
      go func(chTasks chan *common.Task, t *common.Task) {chTasks <- t} (c.tasks, &common.Task{taskID, parameters})
      taskID++
    } else {
      log.Fatal("Fatal while running computation: (%v)\n", err)
    }
  }

  log.Println("Coordinator: finished reading computation tasks")

  c.client.tasksCount = int64(tcount)
}

func (c *Coordinator) sendNextTaskToWorker(wci WCInfo) {
  w, present := c.hash[wci.ID]
  if !present {
    log.Fatalf("sendNextTask: worker with id %v is not registered", wci.ID)
  }

  go w.sendNextTask(wci.sock)
}

func (c *Coordinator) workerTimeoutOccured(hr HealthReporter) {
  id := hr.GetID()
  w, present := c.hash[id]
  if !present {
    log.Fatal("Worker timeout: entity with id #%v is not present in coordinator pool", id)
  }

  if w.pending > 0 {
    // rebalance all existing tasks
    for _, task := range w.activeTasks {
      go func() { c.tasks <- task }()
    }
  }

  delete(c.hash, id)
}

func (c *Coordinator)dispatch(task *common.Task) {
  log.Printf("Dispatching task with id #%v", task.ID)
  addedTask := false

  if len(c.pool) > 0 {
    w := heap.Pop(&c.pool).(*Worker)

    log.Printf("Retrieved worker with ID #%v", w.ID)

    pending := w.pending - w.tasksDone
    log.Printf("Worker pending is %v, tasks done is %v", w.pending, w.tasksDone)
    
    if pending < w.capacity && pending >= 0 {
      w.tasks <- task
      w.IncPendingTasks()
      log.Printf("Added task #%v to worker %v", task.ID, w.ID)
      addedTask = true
    }

    heap.Push(&c.pool, w)
  }

  if (!addedTask) {
    go func(tasks chan *common.Task, t *common.Task) {time.Sleep(1*time.Second); tasks <- t} (c.tasks, task)
  }
}

func (c *Coordinator)refreshDoneStats() {
  statChan := make(chan interface{})
  workersCount := len(c.pool)

  if workersCount == 0 {
    return
  }
  
  for _, w := range c.pool { w.RetrieveStatus(statChan) }

  go func(stat chan interface{}, count int) {
    sum := int64(0)
    reports := 0
    for report := range stat {
      sum += report.(int64)
      reports++
      
      if reports == count {
        // substract number of common parameters for each worker
        sum -= int64(workersCount)
        
        log.Printf("Reporting %v tasks done to client", sum)
        go func(doneStat chan int64, v int64) {doneStat <- v} (c.doneTasksStat, sum)
        return
      }
    }
  }(statChan, workersCount)
}

func (c *Coordinator)updateClientDoneStats(doneTasksCount int64) {
  if c.client != nil {
    c.client.UpdateDoneTasks(doneTasksCount)
  }
}

func (c *Coordinator) setGetResultsFlag() {
  for _, w := range c.pool {
    getresults := w.GetResultsFlagChannel()
    go func(ch chan bool) {ch <- true}(getresults)
  }
}

