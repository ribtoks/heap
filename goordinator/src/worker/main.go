package main

import (
  "log"
  "fmt"
  "os"
  "flag"
  "../common"
)

// Flags
var (
  caddr = flag.String("c", ":54321", "the address of coordinator")
  logfile = flag.String("l", "worker.log", "absolute path to log file")
)

const (
  maxReconnectTries = 100
  defaultBufferLength = 10
)

func main() {
  parseFlags()

  f, err := setupLogging()
  if err != nil {
    defer f.Close()
  }

  cm := &ComputationManager{
    ID: 0,
    healthcheckResponse: make(chan int32),
    statusInfo: make(chan chan int32),
    pendingTasksCount: 0,
    tasks: make(chan *common.Task),
    results: make(map[int64]*common.ComputationResult),
    chResults: make(chan *common.ComputationResult),
    stopMessages: make(chan chan error),
    sendingMode: false,
    stopComputations: make(chan bool, defaultBufferLength),
    computator: &RaytracerComputator{}}

  go cm.processTasks()

  err = initConnection(cm)
  startHealthcheck(cm)

  cm.handleCommands()
}

func parseFlags() {
  flag.Parse()
}

func setupLogging() (f *os.File, err error) {
  f, err = os.OpenFile(*logfile, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666)
  if err != nil {
    fmt.Println("error opening file: %v", *logfile)
    return nil, err
  }

  log.SetOutput(f)
  log.Println("------------------------------")
  log.Println("Worker log started")
  
  return f, err
}

