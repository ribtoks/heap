package main

// #cgo CFLAGS: -I../cpp
// #cgo LDFLAGS: -L../cpp -lraytracer -lstdc++ -lm
// #include "session_interface.h"
import "C"

import "unsafe"

import (
  "image"
  "image/color"
  "os"
  "image/png"
  "math"
)

type CComputationManager struct {
  cm unsafe.Pointer
}

func CCreateComputator(height, width int, objects []float32, objectsNumber int) CComputationManager {
  cObjects := (*C.float)(unsafe.Pointer(&objects[0]))
  cm := C.createComputator(C.int(height), C.int(width), C.float(30), C.int(objectsNumber), cObjects)
  return CComputationManager{cm}
}

func CTraceRays(computator CComputationManager, startIndex, endIndex int64) [][]float32 {
  indicesSize := endIndex - startIndex + 1
  
  results := make([][]float32, indicesSize)
  refResults := make([]*float32, indicesSize)
  
  for i := range(results) {
    data := make([]float32, 3)
    results[i] = data
    refResults[i] = (*float32)(unsafe.Pointer(&data[0]))
  }
  
  resultsPtr := (**C.float)(unsafe.Pointer((&refResults[0])))
  C.traceRays(computator.cm, C.longlong(startIndex), C.longlong(endIndex), resultsPtr)

  return results
}

func CDeleteComputator(computator CComputationManager) {
  C.destroyComputator(computator.cm)
}

func main() {
  width, height := 640, 480
  objects, objectsNumber := createScene()

  computator := CCreateComputator(height, width, objects, objectsNumber)
  defer CDeleteComputator(computator)

  vectorImg := render(computator, height, width, objects)

  println("Render finished. Saving... ")
  
    // save results
  img := image.NewRGBA(image.Rect(0, 0, width, height))
  for i, p := range vectorImg {
    r, g, b := p[0], p[1], p[2]
    x, y := i % width, i / width
    img.SetRGBA(x, y, color.RGBA{
      uint8(math.Min(1.0, float64(r))*255.0),
      uint8(math.Min(1.0, float64(g))*255.0),
      uint8(math.Min(1.0, float64(b))*255.0),
      255})
  }

  toimg, _ := os.Create("scene.png")
  defer toimg.Close()

  png.Encode(toimg, img)

  print("Done")
}

func createScene() ([]float32, int) {
  objectsNumber := 6
  objects := make([]float32, objectsNumber * 12)

  i := 0

  // 0
  i = packToVector(objects, i, 0, -10004, -20) // center
  i = packToVector(objects, i, 10000) // radius
  i = packToVector(objects, i, 0.2, 0.2, 0.2) // surface color
  i = packToVector(objects, i, 0) // reflection
  i = packToVector(objects, i, 0) // transparency
  i = packToVector(objects, i, 0, 0, 0) // emission color

  // 1
  i = packToVector(objects, i, 0, 0, -20) // center
  i = packToVector(objects, i, 4) // radius
  i = packToVector(objects, i, 1.0, 0.32, 0.36) // surface color
  i = packToVector(objects, i, 1) // reflection
  i = packToVector(objects, i, 0.5) // transparency
  i = packToVector(objects, i, 0, 0, 0) // emission color

  // 2
  i = packToVector(objects, i, 5, -1, -15) // center
  i = packToVector(objects, i, 2) // radius
  i = packToVector(objects, i, 0.9, 0.76, 0.46) // surface color
  i = packToVector(objects, i, 1) // reflection
  i = packToVector(objects, i, 0) // transparency
  i = packToVector(objects, i, 0, 0, 0) // emission color

  // 3
  i = packToVector(objects, i, 5, 0, -25) // center
  i = packToVector(objects, i, 3) // radius
  i = packToVector(objects, i, 0.65, 0.77, 0.97) // surface color
  i = packToVector(objects, i, 1) // reflection
  i = packToVector(objects, i, 0) // transparency
  i = packToVector(objects, i, 0, 0, 0) // emission color

  // 4
  i = packToVector(objects, i, -5.5, 0, -15) // center
  i = packToVector(objects, i, 3) // radius
  i = packToVector(objects, i, 0.9, 0.9, 0.9) // surface color
  i = packToVector(objects, i, 1) // reflection
  i = packToVector(objects, i, 0) // transparency
  i = packToVector(objects, i, 0, 0, 0) // emission color

  // light
  // 5
  i = packToVector(objects, i, 0, 20, -30) // center
  i = packToVector(objects, i, 3) // radius
  i = packToVector(objects, i, 0, 0, 0) // surface color
  i = packToVector(objects, i, 0) // reflection
  i = packToVector(objects, i, 0) // transparency
  i = packToVector(objects, i, 3, 3, 3) // emission color

  return objects, objectsNumber
}

func packToVector(objects []float32, index int, items ...float32) int {
  for i, v := range items {
    objects[index + i] = v
  }

  return index + len(items)
}

func render(computator CComputationManager, height, width int, objects []float32) [][]float32 {
  results := CTraceRays(computator, 0, int64(width*height - 1))
  return results
}
