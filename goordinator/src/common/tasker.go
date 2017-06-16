package common

import (
  "encoding/binary"
  "io"
  "log"
)

type Tasker interface {
  GetID() uint32
  GetSubTask(i uint32, gN uint32) Tasker
  GetBinarySize() uint32
  Dump(w io.Writer) error
  Load(r io.Reader) error
}

func WriteTaskers(w io.Writer, params []Tasker) (err error) {
  p := uint32(len(params))
  log.Printf("Sending %v taskers", p)
  err = binary.Write(w, binary.BigEndian, p)
  if err != nil { return err }

  for _, t := range params {
    log.Printf("Sending %v bytes", t.GetBinarySize())
    err = t.Dump(w)
    
    if err != nil {
        return err
    }    
  }

  return nil
}
