package main

import (
  "../common"
  "image"
  "flag"
  "log"
  "os"
  "fmt"
)

// Flags
var (
  caddr = flag.String("c", ":54322", "the address of coordinator")
  input = flag.String("i", "input", "file with input parameters for computations")
  // 4 - fairly chosen by dice roll
  grind = flag.Int("n", 6, "grind number")
  logfile = flag.String("l", "client.log", "absolute path to log file")
)

type ResultsHolder struct {
  tasksSent int
  height, width int
  image *image.RGBA
}

func main() {
  parseFlags()

  f, err := setupLogging()
  if err != nil {
    defer f.Close()
  }

  initConnection()

  canGetResults := make(chan bool, 1)
  startHealthcheck(canGetResults)

  commonParams, err := readCommonParameters() 
  checkFail(err)
  
  err = sendCommonParameters(commonParams)
  checkFail(err)

  height, width := 480, 640
  realParams, err := readRealParameters(height, width)

  var tasksSent int
  tasksSent, err = computeTasks(realParams, *grind)
  checkFail(err)

  <- canGetResults
  err = sendCollectResultsRequest()
  if err != nil {
    return 
  }

  resultsHolder := &ResultsHolder{(tasksSent + 1), height, width, image.NewRGBA(image.Rect(0, 0, width, height))}

  results := make(chan *common.ComputationResult)
  resultsHandled := make(chan bool)
  go handleTaskResults(resultsHolder, results, resultsHandled)
  receiveResults(results)

  <- resultsHandled
  
  saveResults(resultsHolder)
}

func checkFail(err error) {
  if err != nil {
    log.Fatal(err)
  }  
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
  log.Println("Client log started")
  
  return f, err
}

