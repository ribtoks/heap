package main

import (
  "../common"
  "net"
  "log"
  "encoding/binary"
)

type ComputationManager struct {
  // always greater than zero (0 == unassigned)
  ID uint32
  healthcheckResponse chan int32
  statusInfo chan chan int32
  pendingTasksCount int32
  tasks chan *common.Task
  results map[int64]*common.ComputationResult
  chResults chan *common.ComputationResult
  stopMessages chan chan error
  tasksDone int32
  sendingMode bool
  // buffered
  stopComputations chan bool
  computator Computator
}

func (cm *ComputationManager) handleCommands() {
  log.Println("Handle commands loop started")
  var err error
LoopCommands:
  for {
    select {
    case infoChannel := <- cm.statusInfo: infoChannel <- cm.tasksDone
    case pending := <- cm.healthcheckResponse: {
      if pending > cm.pendingTasksCount {
        go cm.downloadNewTask()
        cm.pendingTasksCount++
      } else if pending < 0 && !cm.sendingMode {
        cm.sendingMode = true        
        go cm.sendTaskResults()
      }
    }
    case result := <- cm.chResults: {
      log.Printf("Task execution result received for task with ID #%v", result.ID)
      
      if !cm.sendingMode {
        cm.results[result.ID] = result
        cm.tasksDone = int32(len(cm.results))
      } else {
        log.Fatal("Attempt to save result while sending results")
      }
    }
    case errorChannel := <- cm.stopMessages: {
      errorChannel <- err
      break LoopCommands
    }
    }
  }
}

func (cm *ComputationManager) downloadNewTask() {
  log.Println("Downloading new task...")
  
  conn, _ := net.Dial("tcp", *caddr)

  bw := &common.BinWriter{W: conn}

  bw.Write(common.WGetTask)
  bw.Write(cm.ID)

  if bw.Err != nil {
    return
  }

  var taskID int64
  binary.Read(conn, binary.BigEndian, &taskID)

  log.Printf("Downloading task #%v parameters", taskID)
  
  dataArray, _, _ := common.ReadDataArray(conn)
  // TODO: handle error here

  log.Printf("Received task #%v", taskID)
  cm.tasks <- &common.Task{taskID, dataArray}
}

func (cm *ComputationManager) processTasks() {
  isFirst := true
  
Loop:
  for {
    select {
    case task := <- cm.tasks: {
      if isFirst {
        isFirst = false

        log.Printf("Begin session started")
        err := cm.computator.beginSession(task)
        log.Printf("Begin session finished")

        cm.chResults <- &common.ComputationResult{}

        if err != nil {
          log.Printf("Error on begin session (%v\n)", err)
        }
      } else {
        log.Printf("Task #%v computation started", task.ID)
        cr, err := cm.computator.computeTask(task)
        log.Printf("Task #%v computation finished", task.ID)
        
        if err != nil {
          log.Printf("Error on compute task (%v)\n", err)
        }
        
        cr.ID = task.ID
        cm.chResults <- &cr
      }
    }
      // for healthcheck fail
    case <- cm.stopComputations:
      cm.computator.endSession()
      break Loop
    }
  }
}

func (cm *ComputationManager) sendTaskResults() {
  log.Printf("Sending task results to coordinator..")

  // sequential sending to reduce load on coordinator
  finished := make(chan bool)
  for key, cr := range cm.results {
    go sendTaskResult(cr, finished)
    <- finished
    delete(cm.results, key)    
  }

  go func(f chan bool) {f <- true} (cm.stopComputations)
}

func sendTaskResult(cr *common.ComputationResult, finished chan bool) error {
  defer func(f chan bool) {f <- true}(finished)

  conn, err := net.Dial("tcp", *caddr)
  if err != nil {
    return err
  }

  bw := &common.BinWriter{W: conn}
  bw.Write(common.WSendResult)
  bw.Write(cr.ID)
  bw.Write(cr)

  return bw.Err
}

func (cm *ComputationManager) stop() error {
  log.Printf("ComputationManager: stop received")
  cm.stopComputations <- true
  errors := make(chan error)
  cm.stopMessages <- errors
  return <- errors
}
