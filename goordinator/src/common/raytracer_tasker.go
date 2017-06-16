package common

import (
  "io"
  "math"
)

type SceneSerializer interface {
  Dump(w io.Writer) error
  Load(r io.Reader) error
  GetBinarySize() uint32
}

type Vector3 struct {
  X, Y, Z float32
}

type SphereObject struct {
  Center Vector3
  R, RSqr float32
  SurfaceColor, EmissionColor Vector3
  Transparency, Reflection float32
}

func packToVector(objects []float32, index int, items ...float32) int {
  for i, v := range items {
    objects[index + i] = v
  }

  return index + len(items)
}

func (so *SphereObject) PackToVector(objects []float32, i int) int {
  i = packToVector(objects, i, so.Center.X, so.Center.Y, so.Center.Z)
  i = packToVector(objects, i, so.R)
  i = packToVector(objects, i, so.SurfaceColor.X, so.SurfaceColor.Y, so.SurfaceColor.Z)
  i = packToVector(objects, i, so.Reflection)
  i = packToVector(objects, i, so.Transparency)
  i = packToVector(objects, i, so.EmissionColor.X, so.EmissionColor.Y, so.EmissionColor.Z)

  return i  
}

type BeginSessionTasker struct {
  Width, Height int32
  Angle float32
  ObjectsCount int32
  SceneObjects []float32
  ID uint32
}

func (ft *BeginSessionTasker) GetBinarySize() uint32 {
  return 5*4 + uint32(len(ft.SceneObjects))*4
}

func (ft *BeginSessionTasker) GetID() uint32 {
  return ft.ID
}

func (ft *BeginSessionTasker) GetSubTask(i uint32, gN uint32) Tasker {
  // just skip for begin session tasker
  return ft
}

func (bs *BeginSessionTasker) Dump(w io.Writer) (err error) {
  bw := &BinWriter{W:w}

  // additional 4 bytes for len()
  bw.Write((bs.GetBinarySize() + 4))
  
  bw.Write(bs.Height)
  bw.Write(bs.Width)
  bw.Write(bs.Angle)
  bw.Write(bs.ObjectsCount)
  bw.Write(int32(len(bs.SceneObjects)))
  bw.Write(bs.SceneObjects)
  bw.Write(bs.ID)

  return bw.Err
}

func (bs *BeginSessionTasker) Load(r io.Reader) (err error) {
  br := &BinReader{R: r}

  br.Read(&bs.Height)
  br.Read(&bs.Width)
  br.Read(&bs.Angle)
  br.Read(&bs.ObjectsCount)

  var size int32
  br.Read(&size)
  bs.SceneObjects = make([]float32, size)
  br.Read(&bs.SceneObjects)
  br.Read(&bs.ID)

  return br.Err
}

type ComputeRaysTasker struct {
  StartIndex int64
  EndIndex int64
  ID uint32
}

func (cr *ComputeRaysTasker) Dump(w io.Writer) (err error) {
  bw := &BinWriter{W:w}

  bw.Write(cr.GetBinarySize())

  bw.Write(cr.StartIndex)
  bw.Write(cr.EndIndex)
  bw.Write(cr.ID)

  return bw.Err
}

func (cr *ComputeRaysTasker) Load(r io.Reader) (err error) {
  br := &BinReader{R:r}

  br.Read(&cr.StartIndex)
  br.Read(&cr.EndIndex)
  br.Read(&cr.ID)

  return br.Err
}

func (cr *ComputeRaysTasker) GetID() uint32 { return cr.ID }

func (cr *ComputeRaysTasker) GetSubTask(i uint32, gN uint32) Tasker {
  indicesCount := cr.EndIndex - cr.StartIndex + 1
  
  sliceSz := float64(indicesCount) / float64(gN)
  sliceLength := int64(math.Ceil(sliceSz))
  
  from := int64(i)*sliceLength
  to := int64(i+1)*sliceLength
  if to > cr.EndIndex { to = cr.EndIndex }

  return &ComputeRaysTasker{from, to, 0}
}

func (cr *ComputeRaysTasker) GetBinarySize() uint32 {
  return 8+8+4
}

type RaysComputationResult struct {
  // arrays of rays indices
  // point (x,y) has index (y*imageWidth + x)
  StartIndex int64
  EndIndex int64
  Colors [][]float32
}

func (rcm *RaysComputationResult) Dump(w io.Writer) (err error) {
  bw := &BinWriter{W:w}

  // not writing binary size
  // this method is used just for buffers

  bw.Write(rcm.StartIndex)
  bw.Write(rcm.EndIndex)

  for _, c := range rcm.Colors {
    bw.Write(c[0]); bw.Write(c[1]); bw.Write(c[2])
  }

  return bw.Err
}

func (rcm *RaysComputationResult) Load(r io.Reader) (err error) {
  br := &BinReader{R:r}

  br.Read(&rcm.StartIndex)
  br.Read(&rcm.EndIndex)

  colorsNumber := int(rcm.EndIndex - rcm.StartIndex + 1)
  rcm.Colors = make([][]float32, colorsNumber)

  for i:=0; i < colorsNumber; i++ {
    rcm.Colors[i] = make([]float32, 3)
    var r, g, b float32
    br.Read(&r); br.Read(&g); br.Read(&b);
    rcm.Colors[i][0] = r
    rcm.Colors[i][1] = g
    rcm.Colors[i][2] = b
  }
  
  return br.Err
}


