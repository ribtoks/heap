package main

import (
  "../common"
  "log"
  "net"
  "encoding/binary"
)

func handleWorkersConnections(wch WorkerChannels) {
  log.Printf("Handle worker connections: listening to %v", *lwaddr)
  listener, err := net.Listen("tcp", *lwaddr)

  if err != nil {
    log.Fatal(err)
  }

  // TODO: implement break
  for {
    conn, err := listener.Accept()
    if err != nil {      
      log.Printf("Error while acepting connection (%v)\n", err)
      continue
    }

    sock := common.Socket{conn, make(chan bool)}
    
    go handleWorker(sock, wch)
  }
}

func handleWorker(sock common.Socket, wch WorkerChannels) error {
  log.Printf("Worker connected from %v\n", sock.RemoteAddr())
  
  var opType byte
  var workerID uint32

  err := binary.Read(sock, binary.BigEndian, &opType)
  if err != nil {
    return err
  }

  optype := common.WorkerOperation(opType)
  opTypeStr, knownCode := common.WorkerOperationStr[optype]
  if !knownCode { opTypeStr = "Unknown" }
  
  switch optype {
  case common.WInit:
    log.Println("Adding worker...")
    wch.addworker <- &Worker{
      tasks: make(chan *common.Task, *wcap),
      getResults: make(chan bool),
      ccinfo: make(chan chan interface{}),
      updatePending: make(chan int64),
      activeTasks: make(map[int64]*common.Task),
      capacity: int64(*wcap),
      ID: 0}
    nextID := <- wch.nextID
    binary.Write(sock, binary.BigEndian, nextID)
    go sock.Close()
  case common.WHealthCheck:
    err = binary.Read(sock, binary.BigEndian, &workerID)
    if err == nil {
      log.Printf("Worker healthcheck request received for id #%v", workerID)
      wch.healthcheckRequest <- WCInfo{workerID, &sock}
    }
  case common.WGetTask:
    err = binary.Read(sock, binary.BigEndian, &workerID)
    if err == nil {
      log.Printf("Worker GetTask request received for id #%v", workerID)
      wch.gettaskRequest <- WCInfo{workerID, &sock}
    }
  case common.WTaskCompeted: // can be implemented on demand
  case common.WSendResult:
    log.Printf("Worker sent task result")
    wch.taskresult <- sock
  }

  log.Printf("Waiting for worker connection with code [%v] to finish", opTypeStr)
  <-sock.Done  
  log.Printf("Worker with code [%v] disconnected from %v\n", opTypeStr, sock.RemoteAddr())
  
  return nil
}
