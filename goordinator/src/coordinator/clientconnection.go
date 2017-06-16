package main

import (
  "../common"
  "net"
  "encoding/binary"
  "log"
)

func handleClientsConnections(coordinator *Coordinator, cch ClientChannels) {
  log.Printf("Handle client connections: listening to %v", *lcaddr)
  listener, err := net.Listen("tcp", *lcaddr)
  
  if err != nil {
    log.Fatal(err)
  }

  for {
    conn, err := listener.Accept()
    if err != nil {
      log.Printf("Error while accepting client connection (%v)\n", err)
      continue
    }

    sock := common.Socket{conn, make(chan bool)}

    // TODO: implement break
    go handleClient(sock, coordinator, cch)
  }
}

func handleClient(sock common.Socket, coordinator *Coordinator, cch ClientChannels) error {
  log.Printf("Client connected from %v\n", sock.RemoteAddr())
  
  var opType byte

  err := binary.Read(sock, binary.BigEndian, &opType)
  if err != nil {
    return err
  }

  optype := common.ClientOperation(opType)
  opTypeStr, knownCode := common.ClientOperationStr[optype]
  if !knownCode { opTypeStr = "Unknown" }
  
  switch optype {
  case common.CInitSession:
    cch.addclient <- sock
  case common.CHealthCheck:
    cch.healthcheckRequest <- sock
  case common.CInputParameters:
    cch.readcommondata <- sock
  case common.CRunComputation:
    cch.runcomputation <- sock
  case common.CCollectResults:
    coordinator.collectResults <- true
    go sock.Close()
  case common.CGetResult:
    cch.getresult <- sock
  }

  log.Printf("Waiting for client connection with code [%v] to finish", opTypeStr)
  <-sock.Done
  log.Printf("Client with code [%v] disconnected from %v", opTypeStr, sock.RemoteAddr())
  
  return nil
}
