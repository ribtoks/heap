package main

import "math"

type Sphere struct {
  center Vector3
  r, rSqr float64
  surfaceColor, emissionColor Vector3
  transparency, reflection float64
}

func (s *Sphere) intersect(rayOrig *Vector3, rayDir *Vector3) (t0, t1 float64, success bool) {
  l := s.center.sub(rayOrig)
  tca := l.dot(rayDir)
  if tca < 0 { return 0, 0, false }

  d2 := l.dot(l) - tca*tca
  if d2 > s.rSqr { return 0, 0, false }

  thc := math.Sqrt(s.rSqr - d2)
  t0 = tca - thc
  t1 = tca + thc

  return t0, t1, true
}

func (s *Sphere) normal(intPoint *Vector3) *Vector3 {
  return intPoint.sub(&s.center)
}

func (s *Sphere) getTransparency() float64 {
  return s.transparency
}

func (s *Sphere) getReflection() float64 {
  return s.reflection
}

func (s *Sphere) getSurfaceColor() *Vector3 {
  return &s.surfaceColor
}

func (s *Sphere) getEmissionColor() *Vector3 {
  return &s.emissionColor
}
