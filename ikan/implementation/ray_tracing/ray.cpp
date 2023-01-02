//
//  Ray.cpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#include "ray.hpp"

namespace ikan {
  
  Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
  : origin(origin), direction(direction) {}
  
  glm::vec3 Ray::At(float distance) const {
    return origin + direction * distance;
  }
  
}
