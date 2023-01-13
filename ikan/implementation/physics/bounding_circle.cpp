//
//  sphere.cpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#include "bounding_circle.hpp"

namespace ikan {
  
  BoundingCircle::BoundingCircle() {}
  BoundingCircle::BoundingCircle(const glm::vec3& pos, const glm::vec3& radius, const glm::vec3& rotation) : position(pos), radius(radius), rotation(rotation) { }
  BoundingCircle::BoundingCircle(const BoundingCircle& other) : position(other.position), rotation(other.rotation), radius(other.radius) { }
  BoundingCircle::BoundingCircle(BoundingCircle&& other) : position(other.position), rotation(other.rotation), radius(other.radius) { }
  BoundingCircle& BoundingCircle::operator=(const BoundingCircle& other) {
    position = other.position;
    rotation = other.rotation;
    radius = other.radius;
    return *this;
  }
  BoundingCircle& BoundingCircle::operator=(BoundingCircle&& other) {
    position = other.position;
    rotation = other.rotation;
    radius = other.radius;
    return *this;
  }
  
  BoundingCircle BoundingCircle::GetWorldSpherePos(const glm::mat4& transform) const {
    BoundingCircle Sphere;
    return Sphere;
  }
  
} // namesapce ikan
