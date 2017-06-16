package main

import (
  "../common"
  "flag"
  "log"
  "fmt"
  "os"
)

// Flags
var (
  lwaddr = flag.String("w", ":54321", "the address to listen on for workers connections")
  lcaddr = flag.String("c", ":54322", "the address to listen on for clients connections")
  wcap = flag.Int("p", 5, "worker tasks capacity in range (0, 100)")
  logfile = flag.String("l", "coordinator.log", "absolute path to log file")
)

const (
  maxWorkerTasksCapacity = 100
  initialWorkerCount = 20
)

func main() {
  parseFlags()

  f, err := setupLogging()
  if err != nil {
    defer f.Close()
  }

  workerChannels := WorkerChannels{
    addworker: make(chan *Worker),
    healthcheckRequest: make(chan WCInfo),
    nextID: make(chan uint32),
    gettaskRequest: make(chan WCInfo),
    taskresult: make(chan common.Socket),
    rmworker: make(chan *Worker)}
  
  clientChannels := ClientChannels{
    addclient: make(chan common.Socket),
    healthcheckRequest: make(chan common.Socket),
    readcommondata: make(chan common.Socket),
    runcomputation: make(chan common.Socket),
    getresult: make(chan common.Socket),
    computationResults: make(chan *common.ComputationResult),
    rmclient: make(chan *Client)}
  
  coordinator := &Coordinator{
    pool: make(Pool, 0, initialWorkerCount),
    hash: make(map[uint32]*Worker),
    // buffered
    tasks: make(chan *common.Task, maxWorkerTasksCapacity),
    initWorkerQueue: make(chan *Worker),
    joinPoolQueue: make(chan *Worker),
    collectResults: make(chan bool),
    doneTasksStat: make(chan int64),
    workerTimeout: make(chan HealthReporter),
    clientTimeout: make(chan HealthReporter),
    workerQuit: make(chan bool),
    clientQuit: make(chan bool)}

  go handleClientsConnections(coordinator, clientChannels)
  go handleWorkersConnections(workerChannels)

  go handleWorkerGetResults(workerChannels.taskresult, clientChannels.computationResults)
  go handleClientGetResults(clientChannels.getresult, clientChannels.computationResults)

  go coordinator.handleClientChannels(clientChannels)
  coordinator.handleWorkerChannels(workerChannels)

  coordinator.quit()
}

func parseFlags() {
  flag.Parse()

  if *wcap <= 0 || *wcap > maxWorkerTasksCapacity {
    log.Fatalf("worker tasks capacity must be between 0 and %d non-inclusively", maxWorkerTasksCapacity)
  }  
}

func setupLogging() (f *os.File, err error) {
  f, err = os.OpenFile(*logfile, os.O_RDWR | os.O_CREATE | os.O_APPEND, 0666)
  if err != nil {
    fmt.Println("error opening file: %v", *logfile)
    return nil, err
  }

  log.SetOutput(f)
  log.Println("------------------------------")
  log.Println("Coordinator log started")
  
  return f, err
}
