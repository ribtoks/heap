package common

import (
  "io"
)

// implements Tasker
type TaskParameterFloat struct {
  // to allow efficient memory caching
  Data []float32

  // dim1 - number of 2d arrays
  // dim2*dim3 - dimensions of 2d arrays (height * width)
  Dim1, Dim2, Dim3 uint32

  ID uint32
}

func (ft *TaskParameterFloat) Dump(w io.Writer) (err error) {
  bw := &BinWriter{W:w}

  bw.Write(ft.GetBinarySize())
  bw.Write(ft.Dim1)
  bw.Write(ft.Dim2)
  bw.Write(ft.Dim3)
  bw.Write(ft.Data)

  return bw.Err
}

func (ft *TaskParameterFloat) Load(r io.Reader) (err error) {
  br := &BinReader{R: r}
  br.Read(&ft.Dim1)
  br.Read(&ft.Dim2)
  br.Read(&ft.Dim3)

  ft.Data = make([]float32, ft.Dim1*ft.Dim2*ft.Dim3)
  br.Read(&ft.Data)

  return br.Err
}

func (ft *TaskParameterFloat) GetBinarySize() uint32 {
  sizeofFloat32 := uint32(4)
  dimensionsSize := 3*sizeofFloat32
  dataSize := uint32(ft.Dim1 * ft.Dim2 * ft.Dim3 * sizeofFloat32)
  return dimensionsSize + dataSize
}

func (ft *TaskParameterFloat) GetID() uint32 { return ft.ID }

func (ft *TaskParameterFloat) Get(k, i, j uint32) float32 {
  return ft.Data[(i*ft.Dim2 + j) + ft.Dim2 * ft.Dim3 * k]
}

func (ft *TaskParameterFloat) Get2D(i, j uint32) float32 {
  return ft.Data[i*ft.Dim2 + j]
}

type ExecFunc func(float32, float32) float32

func (ft *TaskParameterFloat) Exec(f ExecFunc, param float32) {
  for i, v := range ft.Data {
    ft.Data[i] = f(v, param)
  }
}

func (ft *TaskParameterFloat) GetSubTask(i uint32, grindNumber uint32) Tasker {
  // TODO: add special case when dim1 == 1
  sliceFrom := i*grindNumber
  sliceTo := (i + 1)*grindNumber

  coef := ft.Dim2*ft.Dim3
  maxIndex := ft.Dim1*ft.Dim2*ft.Dim3
  
  var t Tasker
  from := sliceFrom*coef
  to := sliceTo*coef
  if to > maxIndex { to = maxIndex }

  dim1 := uint32((to - from) / coef)
  
  tpf := &TaskParameterFloat{ft.Data[from:to], dim1, ft.Dim2, ft.Dim3, 0}

  t = tpf
  return t
}
