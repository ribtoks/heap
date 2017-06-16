package main

import (
  "../common"
  "log"
  "fmt"
  "bytes"
  "errors"
)

type MatrixComputator struct {
  factor float32
}

func (mc MatrixComputator) beginSession(task *common.Task) error {
  if len(task.Parameters) != 1 {
    return errors.New("Invalid parameters length")
  }
  
  gd := task.Parameters[0]
  tpf := new(common.TaskParameterFloat)

  r := bytes.NewReader(gd.Data)
  err := tpf.Load(r)
  
  if err != nil {
    return err
  }

  log.Printf("Decoded parameter (%v, %v, %v) with buffer length %v", tpf.Dim1, tpf.Dim2, tpf.Dim3, len(tpf.Data))

  matrixSize := tpf.Dim1 * tpf.Dim2 * tpf.Dim3
  if matrixSize != 1 {
    return errors.New(fmt.Sprintf("Invalid length of initial parameter. %v Expected but %v received", 1, matrixSize))
  }

  mc.factor = tpf.Get(0, 0, 0)
  log.Printf("Received common parameter %v\n", mc.factor)
  return nil
}

func (mc MatrixComputator) computeTask(task *common.Task) (cr common.ComputationResult, err error) {
  cr = common.ComputationResult{}

  if len(task.Parameters) != 1 {
    return cr, errors.New("Invalid parameters")
  }

  gd := task.Parameters[0]

  tpf := new(common.TaskParameterFloat)
  r := bytes.NewReader(gd.Data)
  err = tpf.Load(r)

  if err != nil {
    return cr, err
  }

  var f common.ExecFunc
  f = func(v float32, factor float32) float32 { return v * factor }
  tpf.Exec(f, mc.factor)

  var buf bytes.Buffer
  err = tpf.Dump(&buf)
  
  if err != nil {
    return cr, err
  }

  cr.Data = buf.Bytes()
  cr.Size = uint32(len(cr.Data))

  return cr, nil
}

func (mc MatrixComputator) endSession() error {
  return nil
}
