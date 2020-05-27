#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  BBox bb(p1);
  bb.expand(p2);
  bb.expand(p3);
  return bb;

}


//https://github.com/cmu462/Scotty3D/wiki/Ray-Triangle-Intersection
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool Triangle::intersect(const Ray& r) const {

  // Part 1, Task 3: implement ray-triangle intersection
  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  
  //double epsilon = 0.0000001;

  Vector3D e1 = p2 - p1;        // e1 = p1- p0
  Vector3D e2 = p3 - p1;        // e2 = p2- p0
  Vector3D s = r.o - p1;        // s = o - p0

  //Vector3D pvec = cross(e2, s);       //old
  Vector3D pvec = cross(e2, r.d);
  
  Vector3D e1xd = cross(e1, r.d);
  
  //double det = dot(e1xd, e2);         //old
  double det = dot(e1, pvec);           

  //if(det < epsilon) return false;       //if det is close to zero, it misses triangle

  double invdet = 1/det;

  //double u = dot(pvec, r.d) * invdet;   //old
  double u = dot(s, pvec) * invdet;
  if(u < 0 || u > 1) return false;

  //Vector3D qvec = cross(e1, r.d);       //old
  Vector3D qvec = cross(e1, s);
  
  //double v = dot(qvec, s) * invdet;     //old
  double v = dot(r.d, qvec) * invdet;
  if(v < 0 || v + u > 1) return false;
  
  //double t = dot(pvec, e1) * invdet;      //old
  double t = dot(e2, qvec) * invdet;


  if (t > r.min_t && t < r.max_t) { // if there is an intersection, the if condition should be true
    r.max_t = t; //replace this with your value of t
    return true;
  }

  return false;
}


bool Triangle::intersect(const Ray& r, Intersection *isect) const {

  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  Vector3D n1(mesh->normals[v1]), n2(mesh->normals[v2]), n3(mesh->normals[v3]);

  //double epsilon = 0.0001;

  Vector3D e1 = p2 - p1;        // e1 = p1- p0
  Vector3D e2 = p3 - p1;        // e2 = p2- p0
  Vector3D s = r.o - p1;        // s = o - p0

  //Vector3D pvec = cross(e2, s);       //old
  Vector3D pvec = cross(e2, r.d);
  
  Vector3D e1xd = cross(e1, r.d);
  
  //double det = dot(e1xd, e2);         //old
  double det = dot(e1, pvec);           

  //if(det < epsilon) return false;       //if det is close to zero, it misses triangle

  double invdet = 1/det;

  //double u = dot(pvec, r.d) * invdet;   //old
  double u = dot(s, pvec) * invdet;
  if(u < 0 || u > 1) return false;

  //Vector3D qvec = cross(e1, r.d);       //old
  Vector3D qvec = cross(e1, s);
  
  //double v = dot(qvec, s) * invdet;     //old
  double v = dot(r.d, qvec) * invdet;
  if(v < 0 || v + u > 1) return false;
  
  //double t = dot(pvec, e1) * invdet;      //old
  double t = dot(e2, qvec) * invdet;

  if (t > r.min_t && t < r.max_t) { // if there is an intersection, the if condition should be true

    // b1 is u, b2 is v
    Vector3D normal = (1 - u - v)*n1 + u*n2 + v*n3;     //barycentric coordinates

    isect->t = t; //replace this with your value of t
    isect->primitive = this;
    isect->n = normal; //replace this with your value of normal at the point of intersection
    isect->bsdf = get_bsdf();

    return true;
  }

  return false;
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}



} // namespace StaticScene
} // namespace CGL
