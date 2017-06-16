package main

// #cgo CFLAGS: -I../../../go-ray/src/cpp
// #cgo LDFLAGS: -L../../../go-ray/bin -lraytracer -lstdc++ -lm
// #include "session_interface.h"
import "C"

import (
  "../common"
  "unsafe"
  "bytes"
  "errors"
)


type CComputationManager struct {
  p unsafe.Pointer
}

type RaytracerComputator struct {
  cm CComputationManager
}

func cCreateComputator(height, width int32, angle float32, objects []float32, objectsNumber int32) CComputationManager {
  cObjects := (*C.float)(unsafe.Pointer(&objects[0]))
  cm := C.createComputator(C.int(height), C.int(width), C.float(angle), C.int(objectsNumber), cObjects)
  return CComputationManager{cm}
}

func cTraceRays(computator CComputationManager, startIndex, endIndex int64) [][]float32 {
  indicesSize := endIndex - startIndex + 1
  
  results := make([][]float32, indicesSize)
  refResults := make([]*float32, indicesSize)
  
  for i := range(results) {
    data := make([]float32, 3)
    results[i] = data
    refResults[i] = (*float32)(unsafe.Pointer(&data[0]))
  }
  
  resultsPtr := (**C.float)(unsafe.Pointer((&refResults[0])))
  C.traceRays(computator.p, C.longlong(startIndex), C.longlong(endIndex), resultsPtr)

  return results
}

func cDeleteComputator(computator CComputationManager) {
  C.destroyComputator(computator.p)
}

func (rt *RaytracerComputator) beginSession(task *common.Task) error {
  if len(task.Parameters) != 1 {
    return errors.New("Invalid parameters length")
  }
  
  gd := task.Parameters[0]

  bst := new(common.BeginSessionTasker)
  r := bytes.NewReader(gd.Data)
  err := bst.Load(r)

  if err != nil {
    return err
  }

  rt.cm = cCreateComputator(bst.Height, bst.Width, bst.Angle, bst.SceneObjects, bst.ObjectsCount) 
  return nil
}

func (rt *RaytracerComputator) computeTask(task *common.Task) (cr common.ComputationResult, err error) {
  cr = common.ComputationResult{}

  if len(task.Parameters) != 1 {
    return cr, errors.New("Invalid parameters")
  }

  gd := task.Parameters[0]
  crt := new(common.ComputeRaysTasker)
  r := bytes.NewReader(gd.Data)

  err = crt.Load(r)

  if err != nil {
    return cr, err
  }

  colors := cTraceRays(rt.cm, crt.StartIndex, crt.EndIndex)

  rcr := common.RaysComputationResult{crt.StartIndex, crt.EndIndex, colors}
  
  var buf bytes.Buffer
  err = rcr.Dump(&buf)

  if err != nil {
    return cr, err
  }

  cr.Data = buf.Bytes()
  cr.Size = uint32(len(cr.Data))

  return cr, nil
}

func (rt *RaytracerComputator) endSession() error {
  cDeleteComputator(rt.cm)
  return nil
}
