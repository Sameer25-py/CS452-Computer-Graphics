#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c);
  } else {
    draw(node->l, c);
    draw(node->r, c);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c);
  } else {
    drawOutline(node->l, c);
    drawOutline(node->r, c);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // Part 2, Task 1:
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

/*
  BBox centroid_box, bbox;

  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c);
  }

  BVHNode *node = new BVHNode(bbox);

  node->prims = new vector<Primitive *>(prims);
  return node;
*/



  BBox centroid_box, bbox;

  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c);
  }

  BVHNode *node = new BVHNode(bbox);

  //if prim size is less than max_leaf return else recursively split
  
  if(prims.size() <= max_leaf_size){
    node->prims = new vector<Primitive *>(prims);
    return node;
  }
  else{
    //find largest axis along which we can split the bbox
    int axis = 0;
    //double midpaxis = 0;
    Vector3D bboxVector = centroid_box.extent;             //bboxVector is BBox extent
    double maxval = max(bboxVector.x, max(bboxVector.y, bboxVector.z));

    if(maxval == bboxVector.x){
      axis = 0;
      //midpaxis = (bboxVector.x * 5) + centroid_box.min.x;
    }
    else if(maxval == bboxVector.y){
      axis = 1;
      //midpaxis = (bboxVector.y * 5) + centroid_box.min.y;
    }
    else{
      axis = 2;
      //midpaxis = (bboxVector.z * 5) + centroid_box.min.z;
    }

    vector<Primitive *> prim1 = vector<Primitive *> ();
    vector<Primitive *> prim2 = vector<Primitive *> ();

    double midpaxis = centroid_box.centroid()[axis];      //midpoint point along largest axis for split

    for (Primitive *p : prims) {
      if(p->get_bbox().centroid()[axis] <= midpaxis){
        prim1.push_back(p);
      }
      else{
        prim2.push_back(p);
      }
    }
    //cout << "left called" << endl;
    node->l = construct_bvh(prim1, max_leaf_size);
    //cout << "left returned" << endl;
    //cout << "right called" << endl;
    node->r = construct_bvh(prim2, max_leaf_size);
    //cout << "right returned" << endl;
    
    return node;
  }

}

//check whether ray hits prims
bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {
  // Part 2, task 3: replace this.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  
  
  double tmin=0, tmax=0;

  bool result = node->bb.intersect(ray, tmin, tmax);

  if(result == false){
    return false;
  }
  else if(tmin > tmax){
    return false;
  }
  else if(tmin < ray.min_t || tmin > ray.max_t){
    return false;
  }
  else if(tmax < ray.min_t || tmax > ray.max_t){
    return false;
  }
  else if(node->isLeaf()){
      //bool hitcheck = false;
      //cout << "isleaf entered" << endl;
      for (Primitive *p : *(node->prims)) {
        total_isects++;
        if (p->intersect(ray)) 
          //hitcheck = true;
          return true;
      }
      //cout << "isleaf returned" << endl;
      return false;
      //return hitcheck;
    }
  else{
      bool left = intersect(ray, node->l);
      bool right = intersect(ray, node->r);

      return left || right;
  }  
  
}

//find closest ray hit to prims
bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {
  // Part 2, task 3: replace this

/*
  bool hit = false;
  cout << "enter for loop" << endl;
  for (Primitive *p : *(root->prims)) {
    cout << "isleaf i entered" << endl;
    total_isects++;
    if (p->intersect(ray, i)) 
      cout << "hit true" << endl;
      hit = true;
    cout << "isleaf i returned" << endl;
  }
  return hit;
*/
  
  double tmin=0, tmax=0;

  bool result = node->bb.intersect(ray, tmin, tmax);

  if(result == false){
    return false;
  }
  else if(tmin > tmax){
    return false;
  }
  else if(tmin < ray.min_t || tmin > ray.max_t){
    return false;
  }
  else if(tmax < ray.min_t || tmax > ray.max_t){
    return false;
  }
  else if(node->isLeaf()){
    //cout << "isleaf i entered" << endl;
    bool hit = false;
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray, i))
        //cout << "hit true" << endl;  
        hit = true;
    }
    //cout << "isleaf i returned" << endl;
    return hit;
  }
  else{
    
    bool left = intersect(ray, i, node->l);
    bool right = intersect(ray, i, node->r);

    return left || right;
  }
  
}

}  // namespace StaticScene
}  // namespace CGL
