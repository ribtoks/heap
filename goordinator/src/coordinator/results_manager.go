package main

import (
  "../common"
  "log"
  "encoding/binary"
)

func handleWorkerGetResults(tasksresults <-chan common.Socket, computationResults chan<- *common.ComputationResult) {
  var taskID int64
  var err error

  for sock := range tasksresults {
    err = binary.Read(sock, binary.BigEndian, &taskID)

    if err == nil {
      gd, err := common.ReadGenericData(sock)

      log.Printf("Received task #%v result with size %v", taskID, gd.Size)

      if err == nil {
        go func(crch chan<- *common.ComputationResult, ch *common.ComputationResult) {crch <- ch} (computationResults, &common.ComputationResult{gd, taskID})
      } else {
        log.Printf("Received error while reading task #%v result: %v", taskID, err)
      }
    }

    sock.Close()
  }
}

func handleClientGetResults(getResult <-chan common.Socket, computationResults <-chan *common.ComputationResult) {
  for sock := range getResult {
    cr := <- computationResults
    bw := &common.BinWriter{W: sock}

    bw.Write(cr.ID)
    bw.Write(cr)

    sock.Close()
  }
}
