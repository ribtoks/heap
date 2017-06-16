package common

import "io"
import "encoding/binary"

type BinWriter struct {
  W io.Writer
  Size int64
  Err error
}

func (w *BinWriter) Write(v interface{}) {
  if w.Err != nil {
    return
  }

  switch x := v.(type) {
  case *GenericData:
    w.Err = x.Write(w.W)
  case *ComputationResult:
    w.Err = x.Write(w.W)
  case Tasker:
    w.Err = x.Dump(w.W)
  default:
    if w.Err = binary.Write(w.W, binary.BigEndian, v); w.Err == nil {
      w.Size += int64(binary.Size(v))
    }
  }  
}

type BinReader struct {
  R io.Reader
  Err error
}


func (r *BinReader) Read(v interface{}) {
  if r.Err != nil {
    return
  }

  r.Err = binary.Read(r.R, binary.BigEndian, v)
}
