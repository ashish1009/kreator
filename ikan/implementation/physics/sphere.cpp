//
//  sphere.cpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#include "sphere.hpp"

namespace ikan {
  
  BoundingSphere::BoundingSphere() {}
  BoundingSphere::BoundingSphere(const glm::vec3& pos, float radius) : position(pos), radius(radius) { }
  BoundingSphere::BoundingSphere(const BoundingSphere& other) : position(other.position), radius(other.radius) { }
  BoundingSphere::BoundingSphere(BoundingSphere&& other) : position(other.position), radius(other.radius) { }
  BoundingSphere& BoundingSphere::operator=(const BoundingSphere& other) {
    position = other.position;
    radius = other.radius;
    return *this;
  }
  BoundingSphere& BoundingSphere::operator=(BoundingSphere&& other) {
    position = other.position;
    radius = other.radius;
    return *this;
  }
  
  BoundingSphere BoundingSphere::GetWorldSpherePos(const glm::mat4& transform) const {
    BoundingSphere Sphere;
    return Sphere;
  }
  
} // namesapce ikan
