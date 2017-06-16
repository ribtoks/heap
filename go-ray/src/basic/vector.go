package main

import "math"

type Vector3 struct {
  x,y,z float64
}

func (v *Vector3) lengthSqr() float64 {
  x,y,z := v.x,v.y,v.z
  return x*x + y*y + z*z
}

func (v *Vector3) length() float64 {
  return math.Sqrt(v.lengthSqr())
}

func (v *Vector3) normalize() {
  length := v.lengthSqr()
  if length > 0 {
    inv := 1.0/math.Sqrt(length)
    v.x *= inv; v.y *= inv; v.z *= inv
  }
}

func (v *Vector3) mulF(f float64) *Vector3 {
  return &Vector3{v.x*f, v.y*f, v.z*f}
}

func (a *Vector3) mulV(b *Vector3) *Vector3 {
  return &Vector3{a.x*b.x, a.y*b.y, a.z*b.z}
}

func (a *Vector3) dot(b *Vector3) float64 {
  return a.x*b.x + a.y*b.y + a.z*b.z
}

func (a *Vector3) add(b *Vector3) *Vector3 {
  return &Vector3{a.x+b.x, a.y+b.y, a.z+b.z}
}

func (a *Vector3) sub(b *Vector3) *Vector3 {
  return &Vector3{a.x-b.x, a.y-b.y, a.z-b.z}
}

// A == assign
func (a *Vector3) mulA(b *Vector3) {
  a.x*=b.x; a.y*=b.y; a.z*=b.z
}

func (a *Vector3) addA(b *Vector3) {
  a.x+=b.x; a.y+=b.y; a.z+=b.z
}

func (a *Vector3) neg() *Vector3 {
  return &Vector3{-a.x, -a.y, -a.z}
}

func (a *Vector3) negate() {
  a.x=-a.x; a.y=-a.y; a.z=-a.z
}
