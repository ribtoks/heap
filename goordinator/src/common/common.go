package common

import (
  "net"
  "io"
  "log"
  "encoding/binary"  
)

type WorkerStatus byte
const (
  WReady WorkerStatus = iota
  WIdle
  WBusyAvailable
  WBusyFull
  WFinalized
)

type WorkerOperation byte
const (
  WInit WorkerOperation = iota
  WHealthCheck
  WGetTask
  WTaskCompeted
  WSendResult
)

var WorkerOperationStr = map[WorkerOperation]string {
  WInit: "Init",
  WHealthCheck: "Healthcheck",
  WGetTask: "GetTask",
  WTaskCompeted: "TaskCompleted",
  WSendResult: "SendResult",}

type Task struct {
  ID int64
  Parameters DataArray
}

type Socket struct {
  Conn net.Conn
  Done chan bool
}

func (s Socket) RemoteAddr() net.Addr { return s.Conn.RemoteAddr() }

func (s Socket) Read(b []byte) (int, error) { return s.Conn.Read(b) }
func (s Socket) Write(b []byte) (int, error) { return s.Conn.Write(b) }

func (s Socket) Close() error {
  s.Done <- true
  return nil
}

type ClientStatus byte
const (
  CIdle ClientStatus = iota
  CBusy
)

type ClientOperation byte
const (
  CInitSession ClientOperation = iota
  CHealthCheck
  CInputParameters
  CRunComputation
  CCollectResults
  CGetResult
)

var ClientOperationStr = map[ClientOperation]string{
  CInitSession: "InitSession",
  CHealthCheck: "HealthCheck",
  CInputParameters: "CommonParameters",
  CRunComputation: "RunComputation",
  CCollectResults: "CollectResults",
  CGetResult: "GetResult",}

type GenericData struct {
  Size uint32
  Data []byte
}

func (gd *GenericData) Write(w io.Writer) (err error) {
  bw := &BinWriter{W: w}
  bw.Write(gd.Size)
  bw.Write(gd.Data)
  return bw.Err
}

type DataArray []*GenericData

type ComputationResult struct {
  // if Data is empty, then computation
  // result is considered as Failed
  GenericData
  ID int64
}

func ReadGenericData(r io.Reader) (gd GenericData, err error) {
  var nbytes uint32
  err = binary.Read(r, binary.BigEndian, &nbytes)

  log.Printf("Going to read %v bytes", nbytes)
  
  data := make([]byte, nbytes)
  nread, err := io.ReadFull(r, data)

  log.Printf("Reading finished. Read %v bytes", nread)
  // TODO: handle errors here
  if uint32(nread) == nbytes && err == nil {
    gd = GenericData{nbytes, data}
  }

  return gd, err
}

func ReadDataArray(r io.Reader) (darray DataArray, n int64, err error) {
  var pcount, i uint32

  n = 0

  // number of parameters
  err = binary.Read(r, binary.BigEndian, &pcount)
  n += 4

  // TODO: handle error

  darray = make(DataArray, pcount)
  
  for i = 0; i < pcount; i++ {
    gd, err := ReadGenericData(r)

    if err == nil {
      darray[i] = &gd
      n += int64(gd.Size)
    } else {
      darray[i] = nil
    } 
  }

  return darray, n, err
}

func WriteDataArray(w io.Writer, darray DataArray) (n int64, err error) {
  length := uint32(len(darray))
  err = binary.Write(w, binary.BigEndian, length)
  // TODO: handle errors

  if err != nil {
    return 0, err
  }

  n = 4

Loop:
  for _, gd := range darray {
    err = gd.Write(w)
    if err != nil {
      break Loop
    }

    n += int64(gd.Size)
  }

  return n, err
}
