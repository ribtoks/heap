package main

type Intersecter interface {
  intersect(rayOrig *Vector3, rayDir *Vector3) (t0, t1 float64, success bool)
  normal(intPoint *Vector3) *Vector3
  getTransparency() float64
  getReflection() float64
  getSurfaceColor() *Vector3
  getEmissionColor() *Vector3
}
