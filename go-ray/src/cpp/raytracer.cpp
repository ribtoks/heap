#include "raytracer.h"
#include <cmath>

FloatType mix(const FloatType &a, const FloatType &b, const FloatType &mix)
{
	return b * mix + a * (FloatType(1) - mix);
}

Vector3 getRefraction(const Vector3 &rayDir, const Vector3 &phit, const Vector3 &nhit, bool inside, const std::vector<Intersecter *> &objects, int depth)
{
   Vector3 refraction = 0;
   FloatType ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
   FloatType cos = -nhit.dot(rayDir);
   FloatType k = 1 - eta * eta * (1 - cos * cos);
   Vector3 refractionDir = rayDir * eta + nhit * (eta *  cos - sqrt(k));
   refractionDir.normalize();
   refraction = trace(phit - nhit * BIAS, refractionDir, objects, depth);

   return refraction;
}

Vector3 getReflection(const Vector3 &rayDir, const Vector3 &phit, const Vector3 &nhit, const std::vector<Intersecter *> &objects, int depth, FloatType &fresnelEffect)
{
   FloatType facingratio = -rayDir.dot(nhit);
   // change the mix value to tweak the effect
   fresnelEffect = mix(pow(1 - facingratio, 3), 1, 0.1); 
   // compute reflection direction (not need to normalize because all vectors
   // are already normalized)
   Vector3 refldir = rayDir - nhit * 2 * rayDir.dot(nhit);
   refldir.normalize();
   Vector3 reflection = trace(phit + nhit * BIAS, refldir, objects, depth);
   return reflection;
}

Intersecter *findClosest(const Vector3 &rayOrig, const Vector3 &rayDir, const std::vector<Intersecter *> &objects, FloatType &tnear)
{
   Intersecter *closest = 0;
   
   // find intersection of this ray with the closest in the scene
   for (unsigned i = 0; i < objects.size(); ++i) {
      FloatType t0 = INFINITY, t1 = INFINITY;
      if (objects[i]->Intersect(rayOrig, rayDir, t0, t1)) {
         if (t0 < 0) t0 = t1;
         if (t0 < tnear) {
            tnear = t0;
            closest = objects[i];
         }
      }
   }

   return closest;
}

Vector3 trace(const Vector3 &rayOrig, const Vector3 &rayDir, 
	const std::vector<Intersecter *> &objects, const int &depth)
{
   FloatType tnear = INFINITY;
   const Intersecter *closest = findClosest(rayOrig, rayDir, objects, tnear);

   if (!closest) return Vector3(2);

   Vector3 surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
   Vector3 phit = rayOrig + rayDir * tnear; // point of intersection
   Vector3 nhit = closest->Normal(phit);
   nhit.normalize();
   
   bool inside = rayDir.dot(nhit) > 0;
   if (inside)
   {
      nhit = -nhit;
   }
   
   if ((closest->GetTransparency() > 0 || closest->GetReflection() > 0) && depth < MAX_RAY_DEPTH)
   {
      FloatType fresnelEffect;
      Vector3 reflection = getReflection(rayDir, phit, nhit, objects, depth + 1, fresnelEffect);
      
      Vector3 refraction = 0;
      // if the closest is also transparent compute refraction ray (transmission)
      if (closest->GetTransparency()) {
         refraction = getRefraction(rayDir, phit, nhit, inside, objects, depth + 1);
      }

      surfaceColor = (reflection * fresnelEffect + 
                      refraction * (1 - fresnelEffect) * closest->GetTransparency()) * closest->GetSurfaceColor();
   }
   else {
      for (unsigned i = 0; i < objects.size(); ++i) {
         if (objects[i]->GetEmissionColor().x > 0) {
            // this is a light
            Vector3 transmission = 1;
            Vector3 lightDirection = -(objects[i]->Normal(phit));
            lightDirection.normalize();
            for (unsigned j = 0; j < objects.size(); ++j) {
               if (i != j) {
                  FloatType t0, t1;
                  if (objects[j]->Intersect(phit + nhit * BIAS, lightDirection, t0, t1)) {
                     transmission = 0;
                     break;
                  }
               }
            }
            surfaceColor += closest->GetSurfaceColor() * transmission * 
               std::max(FloatType(0), nhit.dot(lightDirection)) * objects[i]->GetEmissionColor();
         }
      }
   }

   return surfaceColor + closest->GetEmissionColor();
}
