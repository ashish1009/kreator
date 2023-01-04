//
//  scene.hpp
//  ikan
//
//  Created by Ashish . on 01/01/23.
//

#pragma once

#include "ray_material.hpp"
#include "sphere.hpp"

namespace ikan {
  
  class RayScene {
  public:
    std::vector<Sphere> spheres;
    std::vector<RayMaterial> materials;
    
    RayScene() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(RayScene)
  };

}
