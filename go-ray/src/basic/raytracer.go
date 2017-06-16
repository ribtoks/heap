package main

import "math"

const (
  INFINITY = 1e16
  MAX_DEPTH = 5
  BIAS = 1e-4
)

var Background = Vector3{2, 2, 2}

func trace(rayOrig *Vector3, rayDir *Vector3, objects []Intersecter, depth int) *Vector3 {
  closest, tnear := findClosest(rayOrig, rayDir, objects)

  if closest == nil { return &Background}

  // point of intersectino
  phit := rayOrig.add(rayDir.mulF(tnear))
  // normal at the intersection point
  nhit := closest.normal(phit)
  nhit.normalize()

  surfaceColor := &Vector3{}

  // If the normal and the view direction are not opposite to each other 
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.
  inside := rayDir.dot(nhit) > 0
  if inside { nhit.negate() }

  if (closest.getTransparency() > 0 || closest.getReflection() > 0) && depth < MAX_DEPTH {
    reflection, fresnelEffect := getReflection(rayDir, phit, nhit, objects, depth + 1)

    refraction := &Vector3{}
    if closest.getTransparency() != 0 {
      refraction = getRefraction(rayDir, phit, nhit, inside, objects, depth + 1)
    }

    reflectionColor := reflection.mulF(fresnelEffect)
    refractionColor := refraction.mulF((1.0 - fresnelEffect)*closest.getTransparency())
    surfaceColor = reflectionColor.add(refractionColor).mulV(closest.getSurfaceColor())
  } else {
    // it's a diffuse object, no need to raytrace any further
    for i, obj := range objects {
      if obj.getEmissionColor().x > 0 {
        transmission := &Vector3{1, 1, 1}

        lightDirection := obj.normal(phit)
        lightDirection.negate()
        lightDirection.normalize()

        for j, other := range objects {
          if j == i { continue }

          if _, _, ok := other.intersect(phit.add(nhit.mulF(BIAS)), lightDirection); ok {
            transmission = &Vector3{}
            break
          }
        }

        max := math.Max(0.0, nhit.dot(lightDirection))
        colorToAdd := closest.getSurfaceColor().mulV(transmission).mulF(max).mulV(obj.getEmissionColor())
        surfaceColor.addA(colorToAdd)
      }
    }
  }

  return surfaceColor.add(closest.getEmissionColor())
}

func getRefraction(rayDir, phit, nhit *Vector3, inside bool, objects []Intersecter, depth int) *Vector3 {
  ior := 1.1; eta := ior
  if !inside { eta = 1.0/ior }

  cos := -nhit.dot(rayDir)
  k := 1.0 - eta*eta*(1.0 - cos*cos)
  refractionDir := rayDir.mulF(eta).add(nhit.mulF(eta*cos - math.Sqrt(k)))
  refractionDir.normalize()
  refraction := trace(phit.sub(nhit.mulF(BIAS)), refractionDir, objects, depth)

  return refraction
}

func getReflection(rayDir, phit, nhit *Vector3, objects []Intersecter, depth int) (*Vector3, float64) {
  rayDirDotNHit := rayDir.dot(nhit) 
  facingRatio := -rayDirDotNHit
  fresnelEffect := mix(math.Pow(1.0 - facingRatio, 3.0), 1.0, 0.1)

  // compute reflection direction (not need to normalize because all vectors
  // are already normalized)
  reflectionDir := rayDir.sub(nhit.mulF(2*rayDirDotNHit))
  reflectionDir.normalize()

  reflection := trace(phit.add(nhit.mulF(BIAS)), reflectionDir, objects, depth)

  return reflection, fresnelEffect
}

func findClosest(rayOrig *Vector3, rayDir *Vector3, objects []Intersecter) (Intersecter, float64) {
  var closest Intersecter

  tnear := INFINITY
  
  for _, obj := range objects {
    if t0, t1, ok := obj.intersect(rayOrig, rayDir); ok {
      if t0 < 0 { t0 = t1 }
      if t0 < tnear {
        tnear = t0
        closest = obj
      }
    }
  }

  return closest, tnear
}

func mix(a, b, mix float64) float64 {
  return b*mix + a*(1 - mix)
}
