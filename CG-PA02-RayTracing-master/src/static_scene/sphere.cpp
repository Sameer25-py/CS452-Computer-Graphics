#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // Part 1, Task 4:
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  
  Vector3D dir = r.d;     //ray direction
  Vector3D e = r.o;       //ray origin
  Vector3D centre = o;         //circle center
  double radius2 = r2;         //r2 is squared radius
  Vector3D esubtc = e - centre;

  double a = dot(dir, dir);
  double b = 2*dot(dir, esubtc);
  double c = dot(esubtc, esubtc) - (radius2);

  double disc = (b*b) - (4*a*c);
  
  if(disc < 0)  return false;
  else if(disc == 0){
    double t = -b/(2*a);
    //r.max_t = t;
    if(t >= r.min_t && t <= r.max_t){
      t1 = t;
      t2 = t;
      return true;
    } 
  }
  else{
    double tval1 = (-b + sqrt(disc)) / (2*a);
    double tval2 = (-b - sqrt(disc)) / (2*a);

    double tmin = min(tval1, tval2);
    double tmax = max(tval1, tval2);
    
    if(tmin >= r.min_t && tmin <= r.max_t){   //both valid
      t1 = tmin;
      t2 = tmax;
      return true;
    }
  }
  return false;
}

bool Sphere::intersect(const Ray& r) const {

  // Part 1, Task 4:
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1=0, t2=0;
  if(test(r, t1, t2)){
    r.max_t = t1;
    return true;
  }

  return false;
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // Part 1, Task 4:
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1=0, t2=0;
  if(test(r, t1, t2)){
    r.max_t = t1;

    Vector3D normal(r.o + r.d * t1 - o);     //barycentric coordinates, 
    normal.normalize();

    i->t = t1; //replace this with your value of t
    i->primitive = this;
    i->n = normal; //replace this with your value of normal at the point of intersection
    i->bsdf = get_bsdf();

    return true;
  }
  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
