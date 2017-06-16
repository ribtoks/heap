#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include "raytracer.h"

void render(const std::vector<Intersecter *> &objects)
{
   unsigned width = 640, height = 480;
   Vector3 *image = new Vector3[width * height], *pixel = image;
   FloatType invWidth = 1 / FloatType(width), invHeight = 1 / FloatType(height);
   FloatType fov = 30, aspectratio = width / FloatType(height);
   FloatType angle = tan(M_PI * 0.5 * fov / FloatType(180));
   // Trace rays
   for (unsigned y = 0; y < height; ++y) {
      for (unsigned x = 0; x < width; ++x, ++pixel) {
         FloatType xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
         FloatType yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
         Vector3 raydir(xx, yy, -1);
         raydir.normalize();
         *pixel = trace(Vector3(0), raydir, objects, 0);
      }
   }
   // Save result to a PPM image (keep these flags if you compile under Windows)
   std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
   ofs << "P6\n" << width << " " << height << "\n255\n";
   for (unsigned i = 0; i < width * height; ++i) {
      ofs << (unsigned char)(std::min(FloatType(1), image[i].x) * 255) << 
         (unsigned char)(std::min(FloatType(1), image[i].y) * 255) <<
         (unsigned char)(std::min(FloatType(1), image[i].z) * 255); 
   }
   ofs.close();
   delete [] image;
}


int main(int argc, char **argv)
{
   std::vector<Intersecter *> objects;
   // position, radius, surface color, reflectivity, transparency, emission color
   objects.push_back(new Sphere(Vector3(0, -10004, -20), 10000, Vector3(0.2), 0, 0.0));
   objects.push_back(new Sphere(Vector3(0, 0, -20), 4, Vector3(1.00, 0.32, 0.36), 1, 0.5));
   objects.push_back(new Sphere(Vector3(5, -1, -15), 2, Vector3(0.90, 0.76, 0.46), 1, 0.0));
   objects.push_back(new Sphere(Vector3(5, 0, -25), 3, Vector3(0.65, 0.77, 0.97), 1, 0.0));
   objects.push_back(new Sphere(Vector3(-5.5, 0, -15), 3, Vector3(0.90, 0.90, 0.90), 1, 0.0));
   // light
   objects.push_back(new Sphere(Vector3(0, 20, -30), 3, Vector3(0), 0, 0, Vector3(3)));
   render(objects);
   while (!objects.empty()) {
      Sphere *sph = (Sphere*)objects.back();
      objects.pop_back();
      delete sph;
   }

   return 0;
}
