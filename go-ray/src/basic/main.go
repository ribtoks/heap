package main

import (
  "image"
  "image/color"
  "os"
  "image/png"
  "math"
)

func main() {

  objects := createScene()
  
  width, height := 640, 480
  vectorImg := render(width, height, objects)

  print("Render finished. Saving... ")
  
    // save results
  img := image.NewRGBA(image.Rect(0, 0, width, height))
  for i, p := range vectorImg {
    x, y := i % width, i / width
    img.SetRGBA(x, y, color.RGBA{
      uint8(math.Min(1.0, p.x)*255.0),
      uint8(math.Min(1.0, p.y)*255.0),
      uint8(math.Min(1.0, p.z)*255.0),
      255})
  }

  toimg, _ := os.Create("scene.png")
  defer toimg.Close()

  png.Encode(toimg, img)

  print("Done")
}

func createScene() []Intersecter {
  objects := make([]Intersecter, 6)
  objects[0] = &Sphere{
    center: Vector3{0, -10004, -20},
    r: 10000,
    rSqr: 10000*10000,
    surfaceColor: Vector3{0.2, 0.2, 0.2},
    emissionColor: Vector3{},
    transparency: 0,
    reflection: 0,
  }

  objects[1] = &Sphere{
    center: Vector3{0, 0, -20},
    r: 4,
    rSqr: 4*4,
    surfaceColor: Vector3{1.0, 0.32, 0.36},
    emissionColor: Vector3{},
    transparency: 0.5,
    reflection: 1,
  }

  objects[2] = &Sphere{
    center: Vector3{5, -1, -15},
    r: 2,
    rSqr: 2*2,
    surfaceColor: Vector3{0.9, 0.76, 0.46},
    emissionColor: Vector3{},
    transparency: 0,
    reflection: 1,
  }

  objects[3] = &Sphere{
    center: Vector3{5, 0, -25},
    r: 3,
    rSqr: 3*3,
    surfaceColor: Vector3{0.65, 0.77, 0.97},
    emissionColor: Vector3{},
    transparency: 0,
    reflection: 1,
  }

  objects[4] = &Sphere{
    center: Vector3{-5.5, 0, -15},
    r: 3,
    rSqr: 3*3,
    surfaceColor: Vector3{0.9, 0.9, 0.9},
    emissionColor: Vector3{},
    transparency: 0,
    reflection: 1,
  }

  // light
  objects[5] = &Sphere{
    center: Vector3{0, 20, -30},
    r: 3,
    rSqr: 3*3,
    surfaceColor: Vector3{},
    emissionColor: Vector3{3, 3, 3},
    transparency: 0,
    reflection: 0,    
  }

  return objects
}

type ResultPair struct {
  index int
  color *Vector3
}

func render(width, height int, objects []Intersecter) []*Vector3 {
  vectorImg := make([]*Vector3, width * height)
  index := 0

  invWidth, invHeight := 1.0 / float64(width), 1.0 / float64(height)
  fov := 30.0
  aspectRatio := float64(width) / float64(height)
  angle := math.Tan(math.Pi*0.5*fov/180.0)

  results := make(chan ResultPair)

  for y := 0; y < height; y++ {
    for x := 0; x < width; x++ {
      xx := (2*(float64(x)+0.5)*invWidth - 1.0)*angle*aspectRatio
      yy := (1.0 - 2.0*((float64(y)+0.5)*invHeight))*angle

      rayDir := &Vector3{xx, yy, -1}
      rayDir.normalize()

      go func(rd *Vector3, objs []Intersecter, i int, resCh chan ResultPair) {
        color := trace(&Vector3{}, rd, objs, 0)
        resCh <- ResultPair{i, color}
      }(rayDir, objects, index, results)
      
      index++
    }
  }

  count := 0
  maxCount := width * height
  for r := range results {
    vectorImg[r.index] = r.color
    count++

    if count == maxCount { break } 
  }
  
  return vectorImg
}
