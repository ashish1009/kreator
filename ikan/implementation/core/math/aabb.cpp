//
//  aabb.cpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#include "aabb.hpp"

namespace ikan {
  
  AABB::AABB() : min(0.0f), max(0.0f) {}
  AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) { }
  AABB::AABB(const AABB& other) : min(other.min), max(other.max) { }
  AABB::AABB(AABB&& other) : min(other.min), max(other.max) { }
  AABB& AABB::operator=(const AABB& other) {
    min = other.min;
    max = other.max;
    return *this;
  }
  AABB& AABB::operator=(AABB&& other) {
    min = other.min;
    max = other.max;
    return *this;
  }
  
} // namesapce ikan
