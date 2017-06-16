#include "session_interface.h"
#include "raytracer.h"
#include <vector>
#include <cstdlib>
#include <cstdio>

extern "C" {
   void *createComputator(int height, int width, FloatType angle, int objectsNumber, FloatType *objects) {
      RayTracerComputator *computator = new RayTracerComputator(height, width, angle);

      FloatType *sphereData = objects;
      
      for (int i = 0; i < objectsNumber; ++i) {
         Vector3 center(sphereData[0], sphereData[1], sphereData[2]); sphereData += 3;
         FloatType radius = *sphereData; sphereData += 1;
         Vector3 surfaceColor(sphereData[0], sphereData[1], sphereData[2]); sphereData += 3;
         FloatType reflection = *sphereData; sphereData += 1;
         FloatType transparency = *sphereData; sphereData += 1;
         Vector3 emissionColor(sphereData[0], sphereData[1], sphereData[2]); sphereData += 3;

         Sphere *sphere = new Sphere(center, radius, surfaceColor, reflection, transparency, emissionColor);
         computator->PushObject(sphere);
      }
      
      return (void *)computator;
   }

   void traceRays(void *computator, long long startIndex, long long endIndex, float **results) {
      RayTracerComputator *rt = (RayTracerComputator *)computator;

      const std::vector<Intersecter *> sceneObjects = rt->GetSceneObjects();
      long long width = (long long)rt->GetWidth();

      int indicesSize = (int)(endIndex - startIndex + 1);
      long long index = startIndex;
      
      for (int i = 0; index < endIndex; ++i, ++index) {
         long long x = index % width, y = index / width;

         FloatType xx = rt->GetRelativeX((int)x);
         FloatType yy = rt->GetRelativeY((int)y);
         Vector3 raydir(xx, yy, -1);
         raydir.normalize();

         Vector3 v = trace(Vector3(0), raydir, sceneObjects, 0);

         FloatType *result = results[i];
         result[0] = v.x; result[1] = v.y; result[2] = v.z;         
      }
   }

   void destroyComputator(void *p) {
      RayTracerComputator *computator = (RayTracerComputator *)p;

      Intersecter *object = computator->PopObject();

      while (object != 0) {
         Intersecter *objectToDelete = object;
         object = computator->PopObject();

         free(objectToDelete);
      }
      
      free(computator);
   }
}

