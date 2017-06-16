package main

import (
  "../common"
  "log"
  "time"
  "encoding/binary"
)

type HealthReporter interface {
  GetStatus() interface{}
  GetStatusChannel() chan chan interface{}

  SetHealthStatus(status int64)
  GetHealthReply() interface{}
  SetHealthReply(reply int64)

  GetID() uint32

  GetResultsFlagChannel() chan bool
  SetGetResultsFlag()

  GetUpdateReplyChannel() chan int64
}

func checkHealth(hr HealthReporter, sock *common.Socket, timeout chan HealthReporter) {
  defer sock.Close()

  healthcheck := make(chan int32, 1)
  reply := make(chan interface{})
  done := make(chan bool, 1)

  go func() {
    // means tasksDone for worker
    var heartBeat int32
    
    for {
      err := binary.Read(sock, binary.BigEndian, &heartBeat)
      if err != nil {
        log.Printf("Error while reading heartbeat %v. Exiting...", err)
        return
      }

      healthcheck <- heartBeat

      select {
      case healthReply := <- reply: {
        err := binary.Write(sock, binary.BigEndian, healthReply)
        if err != nil {
          // TODO: send errors to channel
          log.Printf("Error while sending healthcheck reply (%v)", err)
          return
        }
      }
      case <- done: return
      }
    }
  }()

  statusChannel := hr.GetStatusChannel()
  resultsFlagChannel := hr.GetResultsFlagChannel()

  updateReplyChannel := hr.GetUpdateReplyChannel()

  Loop:
  for {
    select {
    case heartBeat := <- healthcheck: {
      hr.SetHealthStatus(int64(heartBeat))
      reply <- hr.GetHealthReply()
    }
    case update := <- updateReplyChannel: hr.SetHealthReply(update)
    case hchannel := <- statusChannel: hchannel <- hr.GetStatus()
    case <- resultsFlagChannel: hr.SetGetResultsFlag()
    case <- time.After(5 * time.Second): {
      log.Printf("Timeout in healthcheck")
      // TODO: change timeout
      done <- true
      timeout <- hr
      break Loop
    }
    }
  }
}
