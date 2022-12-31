//
//  HitPayload.hpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"

namespace ray_tracing {

  struct HitPayload {
    float hit_distance;
    glm::vec3 world_normal;
    glm::vec3 world_position;
  };

}
