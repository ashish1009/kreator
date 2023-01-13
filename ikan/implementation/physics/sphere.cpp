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
    // Update the Bounding box based on the transform of the mesh
    Sphere.position = glm::vec3(transform * glm::vec4(Sphere.position.x, Sphere.position.y, Sphere.position.z, 1.0));
    return Sphere;
  }
  
} // namesapce ikan
