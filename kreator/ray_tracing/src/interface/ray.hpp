//
//  ray.hpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#pragma once

/// Basic ray functionality. Stores the property of camera ray 
struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
  mutable float near_plane = 0.01f;
  mutable float far_plane = 10000.0f;
};

