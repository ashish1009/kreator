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
  
  BoundingCircle BoundingCircle::GetWorldSpherePos(const glm::vec3& position,
                                                   const glm::vec3& rotation,
                                                   const glm::vec3& scale) {
    BoundingCircle circle;
    circle.position = position;
    circle.radius = scale / 2.0f;
    circle.rotation = rotation;
    return circle;
  }
  
} // namesapce ikan
