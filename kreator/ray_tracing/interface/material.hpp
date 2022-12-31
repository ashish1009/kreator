//
//  material.hpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

namespace ray_tracing {
  
  struct Material {
    glm::vec3 albedo;
    float roughness = 1.0f;
    float metallic = 0.0f;
  };
  
}
