//
//  scene.hpp
//  ray_tracing
//
//  Created by Ashish . on 01/01/23.
//

#pragma once

#include "ray_material.hpp"
#include "sphere.hpp"

namespace ray_tracing {
  
  class RayScene {
  public:
    std::vector<Sphere> spheres;
    std::vector<Material> materials;
  };

}
