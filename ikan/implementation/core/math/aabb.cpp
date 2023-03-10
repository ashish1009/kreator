//
//  aabb.cpp
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#include "aabb.hpp"

namespace ikan {
  
  AABB::AABB() : min(-0.5f), max(0.5f) {}
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
  
  AABB AABB::GetWorldAABBPos(const glm::mat4& transform) {
    AABB aabb;
    // Update the Bounding box based on the transform of the mesh
    aabb.min = glm::vec3(transform * glm::vec4(-0.5f, -0.5f, -0.5f, 1.0));
    aabb.max = glm::vec3(transform * glm::vec4(0.5f, 0.5f, 0.5f, 1.0));
    return aabb;
  }
  
} // namesapce ikan
