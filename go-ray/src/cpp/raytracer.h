#ifndef __RAYTRACER_HEADER
#define __RAYTRACER_HEADER

#include <vector>
#include <cmath>

#ifndef INFINITY
#define INFINITY 1e8
#endif

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#define MAX_RAY_DEPTH 5
#define BIAS 1e-4

typedef float FloatType;

class Vector3
{
public:
   FloatType x, y, z;
   Vector3() : x(FloatType(0)), y(FloatType(0)), z(FloatType(0)) {}
   Vector3(FloatType xx) : x(xx), y(xx), z(xx) {}
   Vector3(FloatType xx, FloatType yy, FloatType zz) : x(xx), y(yy), z(zz) {}
   Vector3& normalize()
   {
      FloatType nor2 = lengthSqr();
      if (nor2 > 0) {
         FloatType invNor = 1 / sqrt(nor2);
         x *= invNor, y *= invNor, z *= invNor;
      }
      return *this;
   }
   Vector3 operator * (const FloatType &f) const { return Vector3(x * f, y * f, z * f); }
   Vector3 operator * (const Vector3 &v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
   FloatType dot(const Vector3 &v) const { return x * v.x + y * v.y + z * v.z; }
   Vector3 operator - (const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
   Vector3 operator + (const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
   Vector3& operator += (const Vector3 &v) { x += v.x, y += v.y, z += v.z; return *this; }
   Vector3& operator *= (const Vector3 &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
   Vector3 operator - () const { return Vector3(-x, -y, -z); }
   FloatType lengthSqr() const { return x * x + y * y + z * z; }
   FloatType length() const { return sqrt(lengthSqr()); }
};

class Intersecter
{
public:
   virtual bool Intersect(const Vector3 &rayOrig, const Vector3 &rayDir, FloatType &t1, FloatType &f2) const = 0;
   virtual Vector3 Normal(const Vector3 &intersectPoint) const = 0;
   virtual FloatType GetTransparency() const = 0;
   virtual FloatType GetReflection() const = 0;
   virtual const Vector3 &GetSurfaceColor() const = 0;
   virtual const Vector3 &GetEmissionColor() const = 0;
};

class Sphere : public Intersecter
{
public:
   Vector3 center;                         /// position of the sphere
   FloatType radius, radius2;                      /// sphere radius and radius^2
   Vector3 surfaceColor, emissionColor;    /// surface color and emission (light)
   FloatType transparency, reflection;             /// surface transparency and reflectivity
   Sphere(const Vector3 &c, const FloatType &r, const Vector3 &sc, 
          const FloatType &refl = 0, const FloatType &transp = 0, const Vector3 &ec = 0) : 
      center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
      transparency(transp), reflection(refl)
   {}
   // compute a ray-sphere intersection using the geometric solution
   virtual bool Intersect(const Vector3 &rayorig, const Vector3 &raydir, FloatType &t0, FloatType &t1) const
   {
      Vector3 l = center - rayorig;
      FloatType tca = l.dot(raydir);
      if (tca < 0) return false;
      FloatType d2 = l.dot(l) - tca * tca;
      if (d2 > radius2) return false;
      FloatType thc = sqrt(radius2 - d2);
      t0 = tca - thc;
      t1 = tca + thc;
      
      return true;
   }

   virtual Vector3 Normal(const Vector3 &intersectPoint) const
   {
      return intersectPoint - this->center;
   }
   
   virtual FloatType GetTransparency() const { return transparency; }
   virtual FloatType GetReflection() const { return reflection; }
   virtual const Vector3 &GetSurfaceColor() const { return surfaceColor; }
   virtual const Vector3 &GetEmissionColor() const { return emissionColor; }
};

struct RayTracerComputator
{
public:
   RayTracerComputator(int height, int width, FloatType angle) :
      m_SceneHeight(height),
      m_SceneWidth(width),
      m_ViewAngle(tan(M_PI * FloatType(0.5) * angle / FloatType(180))),
      m_InvertedHeight(1/(FloatType)height),
      m_InvertedWidth(1/(FloatType)width),
      m_AspectRatio((FloatType)width/(FloatType)height)
   {}

   const std::vector<Intersecter *> &GetSceneObjects() const { return m_SceneObjects; }
   const FloatType GetWidth() const { return m_SceneWidth; }
   const FloatType GetHeight() const { return m_SceneHeight; }

   void PushObject(Intersecter *object) { m_SceneObjects.push_back(object); }

   Intersecter *PopObject() {
      Intersecter *obj = 0;
      if (m_SceneObjects.size() > 0) {
         obj = m_SceneObjects.back();
         m_SceneObjects.pop_back();
      }

      return obj;
   }

   FloatType GetRelativeX(int x) {
      return (2 * ((x + FloatType(0.5)) * m_InvertedWidth) - 1) * m_ViewAngle * m_AspectRatio;
   }

   FloatType GetRelativeY(int y) {
      return (1 - 2 * ((y + FloatType(0.5)) * m_InvertedHeight)) * m_ViewAngle;
   }
   
private:
   std::vector<Intersecter *> m_SceneObjects;
   int m_SceneWidth;
   int m_SceneHeight;
   // in degrees
   FloatType m_ViewAngle;
   FloatType m_InvertedWidth;
   FloatType m_InvertedHeight;
   FloatType m_AspectRatio;
};

Vector3 trace(const Vector3 &rayOrig, const Vector3 &rayDir, 
              const std::vector<Intersecter *> &objects, const int &depth);

#endif // __RAYTRACER_HEADER
