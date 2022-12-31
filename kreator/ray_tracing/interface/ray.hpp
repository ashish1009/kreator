//
//  Ray.hpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

namespace ray_tracing {
  
  /// Basic Ray class to store the data for each ray
  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 At(float distance);
    
    Ray() = default;
    Ray(const glm::vec3& origin, const glm::vec3& direction);
  };
  
}
