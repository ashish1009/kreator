//
//  Ray.cpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#include "ray.hpp"

namespace ikan {
  
  Ray& Ray::operator=(const Ray &other) {
    IK_CORE_TRACE("Ray", "Copying Ray ...");
    origin = other.origin;
    direction = other.direction;
    return *this;
  }
  
  Ray& Ray::operator=(Ray&& other) {
    IK_CORE_TRACE("Ray", "Moving Ray ...");
    origin = other.origin;
    direction = other.direction;
    return *this;
  }
  
  Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
  : origin(origin), direction(direction) {}
  
  glm::vec3 Ray::At(float distance) const {
    return origin + direction * distance;
  }
  
}
