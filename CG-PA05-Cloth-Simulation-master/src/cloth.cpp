#include <iostream>
#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.

  //building grid
  double unitWidth = width/num_width_points;
  double unitHeight = height/num_height_points;

  //cout << "build grid" << endl;
  for(int col = 0; col < num_height_points; col++){                 //row wise insertion
    for (int row = 0; row < num_width_points; row++){
      Vector3D position;
      bool pointMassPin;

      double posCol = unitHeight * col;
      double posRow = unitWidth * row;
      if(orientation == 0){
        position = Vector3D(posRow, 1.0, posCol);
      }
      else{
        double randVal = (double) rand() / RAND_MAX;
        double z = (-0.001) + (randVal * 0.002);
        position = Vector3D(posRow, posCol, z); 
      }

      //bool inPinned = (find(pinned.begin(), pinned.end(), position) != pinned.end());
      if((find(pinned.begin(), pinned.end(), vector<int>{row, col}) != pinned.end())){
        pointMassPin = true;

        PointMass pm = PointMass(position, pointMassPin);
        point_masses.push_back(pm);
      }
      else{
        pointMassPin = false;
        PointMass pm = PointMass(position, pointMassPin);
        point_masses.push_back(pm);
      }
    }
  }

  //Create Springs

  //cout << "create springs" << endl;
  for(int posCol = 0; posCol < num_height_points; posCol++){
    for(int posRow = 0; posRow < num_width_points; posRow++){
      int index = (posCol * num_height_points) + posRow;
      int leftIndex = (posCol * num_height_points) + (posRow-1);
      int rightIndex = (posCol * num_height_points) + (posRow+1);
      int aboveIndex = ((posCol-1) * num_height_points) + posRow;
      int twoAboveIndex = ((posCol-2) * num_height_points) + posRow;
      int twoRightIndex = (posCol * num_height_points) + (posRow+2);
      int upperDiagLeft = ((posCol-1) * num_height_points) + (posRow-1);
      int upperDiagRight = ((posCol-1) * num_height_points) + (posRow+1);


      //structural constraint
      //left
      if((posRow-1 >= 0 && posRow-1 < num_width_points) && (posCol >= 0 && posCol < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[leftIndex], STRUCTURAL);
        springs.push_back(s);
      }

      //above
      if((posRow >= 0 && posRow < num_width_points) && (posCol-1 >= 0 && posCol-1 < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[aboveIndex], STRUCTURAL);
        springs.push_back(s);
      }

      //shearing constraint
      //diagonal upper left
      if((posRow-1 >= 0 && posRow-1 < num_width_points) && (posCol-1 >= 0 && posCol-1 < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[upperDiagLeft], SHEARING);
        springs.push_back(s);
      }

      //diagonal upper right
      if((posRow+1 >= 0 && posRow+1 < num_width_points) && (posCol-1 >= 0 && posCol-1 < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[upperDiagRight], SHEARING);
        springs.push_back(s);
      }

      //bending constraints
      //two point away to right
      if((posRow+2 >= 0 && posRow+2 < num_width_points) && (posCol >= 0 && posCol < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[twoRightIndex], BENDING);
        springs.push_back(s);
      }

      //two point away to above
       if((posRow >= 0 && posRow < num_width_points) && (posCol-2 >= 0 && posCol-2 < num_height_points)){
        Spring s = Spring(&point_masses[index], &point_masses[twoAboveIndex], BENDING);
        springs.push_back(s);
      }
    }
  }
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;


  // TODO (Part 2): Compute total force acting on each point mass.

  //total external force
  Vector3D totalAcceleration = Vector3D(0,0,0);
  Vector3D externalForce = Vector3D(0,0,0);

  for(Vector3D &singleA : external_accelerations){
    totalAcceleration += singleA;
  }

  externalForce = mass * totalAcceleration;

  for(PointMass &singlePm : point_masses){
    singlePm.forces = externalForce;
  }

  //spring correction forces
  for(Spring &sp : springs){
    Vector3D posDiff = sp.pm_a->position - sp.pm_b->position;
    double springRL = sp.rest_length;

    double magPosDiff = pow((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y) + (posDiff.z * posDiff.z), 0.5);
    Vector3D springForce = cp->ks * (magPosDiff - springRL);
    Vector3D magSpringForce = pow((springForce.x * springForce.x) + (springForce.y * springForce.y) + (springForce.z * springForce.z), 0.5);
    
    if(cp->enable_structural_constraints && sp.spring_type == STRUCTURAL){
      sp.pm_a->forces += magSpringForce;
      sp.pm_b->forces += (-magSpringForce);
    }

    if(cp->enable_shearing_constraints && sp.spring_type == SHEARING){
      sp.pm_a->forces += magSpringForce;
      sp.pm_b->forces += (-magSpringForce);
    }

    if(cp->enable_bending_constraints && sp.spring_type == BENDING){
      sp.pm_a->forces += magSpringForce;
      sp.pm_b->forces += (-magSpringForce);
    }
  }


  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for(PointMass &singlePm : point_masses){
    if(singlePm.pinned == 0){
      double dampTerm = cp->damping/100.0;
      double squareDeltaT = delta_t * delta_t;
      Vector3D a = singlePm.forces / mass;

      Vector3D newPosition = (singlePm.position) + ((1.0-dampTerm)* (singlePm.position - singlePm.last_position)) + (a * squareDeltaT);
      singlePm.last_position = singlePm.position;
      singlePm.position = newPosition;
    }
  }


  // TODO (Part 4): Handle self-collisions.
  // This won't do anything until you complete Part 4.
  build_spatial_map();
  for (PointMass &pm : point_masses) {
    self_collide(pm, simulation_steps);
  }


  // TODO (Part 3): Handle collisions with other primitives.
  // This won't do anything until you complete Part 3.
  for (PointMass &pm : point_masses) {
    for (CollisionObject *co : *collision_objects) {
      co->collide(pm);
    }
  }


  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  /*
  for(Spring &sp : springs){
    Vector3D posDiff = sp.pm_a->position - sp.pm_b->position;
    double springRL = sp.rest_length;

    double springLength = pow((posDiff.x * posDiff.x) + (posDiff.y * posDiff.y) + (posDiff.z * posDiff.z), 0.5);
    //double springLength = posDiff / magPosDiff;
    double distance = springLength - sp.rest_length * 1.1;

    //both are pinnned 

    //a in pinned

    //b is pinned
  }
  */
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.

}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.

}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents
  // membership in some uniquely identified 3D box volume.

  return 0.f;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm, pm + num_width_points, pm + 1));
      triangles.push_back(new Triangle(pm + 1, pm + num_width_points,
                                       pm + num_width_points + 1));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
