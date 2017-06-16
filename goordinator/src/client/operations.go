package main

import (
  "../common"
  "encoding/binary"
  "image/color"
  "math"
  "net"
  "bytes"
  "log"
  "time"
  "os"
  "image/png"
)

func readCommonParameters() (params []common.Tasker, err error) {
  params = make([]common.Tasker, 1)

  objects := make([]common.SphereObject, 6)
  objects[0] = common.SphereObject{
    Center: common.Vector3{0, -10004, -20},
    R: 10000,
    SurfaceColor: common.Vector3{0.2, 0.2, 0.2},
    EmissionColor: common.Vector3{},
    Transparency: 0,
    Reflection: 0,
  }

  objects[1] = common.SphereObject{
    Center: common.Vector3{0, 0, -20},
    R: 4,
    SurfaceColor: common.Vector3{1.0, 0.32, 0.36},
    EmissionColor: common.Vector3{},
    Transparency: 0.5,
    Reflection: 1,
  }

  objects[2] = common.SphereObject{
    Center: common.Vector3{5, -1, -15},
    R: 2,
    SurfaceColor: common.Vector3{0.9, 0.76, 0.46},
    EmissionColor: common.Vector3{},
    Transparency: 0,
    Reflection: 1,
  }

  objects[3] = common.SphereObject{
    Center: common.Vector3{5, 0, -25},
    R: 3,
    SurfaceColor: common.Vector3{0.65, 0.77, 0.97},
    EmissionColor: common.Vector3{},
    Transparency: 0,
    Reflection: 1,
  }

  objects[4] = common.SphereObject{
    Center: common.Vector3{-5.5, 0, -15},
    R: 3,
    SurfaceColor: common.Vector3{0.9, 0.9, 0.9},
    EmissionColor: common.Vector3{},
    Transparency: 0,
    Reflection: 1,
  }

  // light
  objects[5] = common.SphereObject{
    Center: common.Vector3{0, 20, -30},
    R: 3,
    SurfaceColor: common.Vector3{},
    EmissionColor: common.Vector3{3, 3, 3},
    Transparency: 0,
    Reflection: 0,    
  }

  bst := &common.BeginSessionTasker{
    Width: 640,
    Height: 480,
    Angle: 60,
    ObjectsCount: 6}

  bst.SceneObjects = make([]float32, 12*len(objects))
  i := 0
  for _, o := range objects {
    i = o.PackToVector(bst.SceneObjects, i)
  }

  params[0] = bst
  return params, nil
}

func readRealParameters(height, width int) (params []common.Tasker, err error) {
  params = make([]common.Tasker, 1)
  params[0] = &common.ComputeRaysTasker{0, int64(height*width - 1), 0}
  return params, nil
}

func initConnection() {
  log.Println("Waiting for a coordinator connection...")
  
Loop:
  for {
    log.Printf("Connecting to coordinator %v...\n", *caddr)
    conn, err := net.Dial("tcp", *caddr)

    if err == nil {
      log.Println("Connected successfully to coordinator")
      binary.Write(conn, binary.BigEndian, common.CInitSession)

      var success byte
      err = binary.Read(conn, binary.BigEndian, &success)

      if err == nil && success == 1 {
        break Loop
      }
    }

    time.Sleep(2 * time.Second)
  }

  log.Println("Reconnect loop finished...")
}

func startHealthcheck(canGetResults chan bool) {
  conn, err := net.Dial("tcp", *caddr)

  if err != nil {
    log.Printf("Unable to connect to coordinator.. exiting")
    return
  } else {
    log.Printf("Connected to coordinator. Sending healthcheck session request")
  }

  err = binary.Write(conn, binary.BigEndian, common.CHealthCheck)
  if err != nil {
    log.Printf("Unable to init healthcheck session")
    return
  }

  go healthcheckMainLoop(conn, canGetResults)
}

func healthcheckMainLoop(conn net.Conn, canGetResults chan bool) {
  notified := false
  
  for {
    second := time.After(1*time.Second)

    heartBeat := uint32(common.CIdle)
    binary.Write(conn, binary.BigEndian, heartBeat)

    var percentage int32
    // TODO: handle error
    err := binary.Read(conn, binary.BigEndian, &percentage)

    if err == nil {
      log.Printf("Percentage is %v", percentage)
      
      if percentage == 100 && !notified {
        log.Printf("Notifying abour results")
        canGetResults <- true
        notified = true
      }
    } else {
      log.Fatal(err)
    }

    <- second
  }
}

func sendCommonParameters(params []common.Tasker) error {
  conn, err := startCommonParamsConnection()
  if err != nil { return err }

  common.WriteTaskers(conn, params)
  
  if err == nil {
    log.Println("Common parameters have been sent successfully")
  } else  {
    log.Print("Failed to send part of generic data, error (%v)\n", err)
  }

  return err
}

func startCommonParamsConnection() (conn net.Conn, err error) {
  conn, err = net.Dial("tcp", *caddr)

  if err != nil {
    log.Printf("Unable to connect to coordinator.. exiting")
    return nil, err
  } else {
    log.Printf("Sending common parameters")
  }

  err = binary.Write(conn, binary.BigEndian, common.CInputParameters)
  if err != nil {
    log.Printf("Unable to init healthcheck session")
    return nil, err
  }

  return conn, nil
}

func computeTasks(parameters []common.Tasker, grindNumber int) (int, error) {
  conn, err := startComputeConnection()
  if err != nil { return 0, err }

  bw := common.BinWriter{W: conn}

  paramLength := uint32(len(parameters))
  log.Printf("Sending grinded (grind number %v) parameters to coordinator...", grindNumber)

  log.Printf("Sending batches count (%v)", grindNumber)
  bw.Write(uint32(grindNumber))

  tasksSent := 0

  for i:=0; i < grindNumber; i++ {
    bw.Write(paramLength)

    for j, p := range parameters {
      subtask := p.GetSubTask(uint32(i), uint32(grindNumber))
      log.Printf("Dumping subtask %v of parameter %v (size = %v)", j, i, subtask.GetBinarySize())
      
      bw.Write(subtask)

      if bw.Err != nil {
        log.Printf("Error while dumping subtask %v of parameter %v (%v)\n", i, j, bw.Err)
        return tasksSent, bw.Err
      }

      tasksSent += 1
    }
  }

  log.Println("Sending tasks loop finished")

  return tasksSent, bw.Err
}

func startComputeConnection() (conn net.Conn, err error) {
  conn, err = net.Dial("tcp", *caddr)
  
  if err != nil {
    log.Printf("Unable to connect to coordinator.. exiting")
    return nil, err
  } else {
    log.Printf("Sending main parameters")
  }

  err = binary.Write(conn, binary.BigEndian, common.CRunComputation)
  if err != nil {
    log.Printf("Unable to start main computation session (%v)\n", err)
    return nil, err
  }

  return conn, nil
}

func sendCollectResultsRequest() error {
  conn, err := net.Dial("tcp", *caddr)
  
  if err == nil {
    err = binary.Write(conn, binary.BigEndian, common.CCollectResults)
  }

  if err != nil {
    log.Printf("Failed to ask coordinator to collect results")
  }

  return err
}

func receiveResults(results chan *common.ComputationResult) {
  // in this version
  queuesNumber := 2
  
  for ; queuesNumber > 0; queuesNumber-- {
    go receiveResultsLoop(results)
  }
}

func receiveResultsLoop(results chan *common.ComputationResult) {
  ping := make(chan bool)
getResults:
  for {
    go getOneResult(results, ping)

    // wait for coordinator connection
    select {
    case <- ping:
    case <- time.After(5*time.Second):
      break getResults
    }

    // wait for reading task itself
    select {
    case <- ping:
      // TODO: make these numbers constants
    case <- time.After(1 * time.Minute):
      break getResults
    }
  }
}

func getOneResult(results chan *common.ComputationResult, ping chan bool) error {
  conn, err := net.Dial("tcp", *caddr)
  if err != nil {
    return err
  }

  err = binary.Write(conn, binary.BigEndian, common.CGetResult)
  if err != nil {
    return err
  }

  ping <- true

  var taskID int64
  err = binary.Read(conn, binary.BigEndian, &taskID)

  var gd common.GenericData
  
  if err == nil {
    gd, err = common.ReadGenericData(conn)
    ping <- true

    if err == nil {
      go func(rs chan *common.ComputationResult, cr *common.ComputationResult) {rs <- cr} (results, &common.ComputationResult{gd, taskID})
    }
  }

  return nil
}

func handleTaskResults(rh *ResultsHolder, results chan *common.ComputationResult, handled chan bool) {
  resultsReceived := 0
  
saveLoop:
  for {
    select {
    case cr := <- results: {
      saveTaskResult(rh, cr)
      resultsReceived++

      log.Printf("Results received %v out of %v\n", resultsReceived, rh.tasksSent)

      if resultsReceived == rh.tasksSent {
        break saveLoop
      }
    }
    case <- time.After(1 * time.Minute): break saveLoop
    }
  }

  log.Printf("Handled all task results...")
  handled <- true
}

func saveTaskResult(rh *ResultsHolder, cr *common.ComputationResult) {
  log.Printf("Received task result for task #%v with size %v", cr.ID, cr.Size)

  rcr := new(common.RaysComputationResult)
  r := bytes.NewReader(cr.Data)
  err := rcr.Load(r)

  width := int64(rh.width)

  if err != nil {
    log.Printf("Failed to decode rays computation task result")
    return
  }

  for i:=rcr.StartIndex; i <= rcr.EndIndex; i++ {
    p := rcr.Colors[i - rcr.StartIndex]
    r, g, b := p[0], p[1], p[2]

    x, y := i % width, i / width
    rh.image.SetRGBA(int(x), int(y), color.RGBA{
      uint8(math.Min(1.0, float64(r))*255.0),
      uint8(math.Min(1.0, float64(g))*255.0),
      uint8(math.Min(1.0, float64(b))*255.0),
      255})
  }
}

func saveResults(rh *ResultsHolder) {
  log.Println("Saving results...")
  
  toimg, _ := os.Create("scene.png")
  defer toimg.Close()

  png.Encode(toimg, rh.image)
}
