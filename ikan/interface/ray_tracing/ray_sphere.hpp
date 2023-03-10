//
//  sphere.hpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"
#include "hit_payload.hpp"
#include "ray_material.hpp"

namespace ikan {
  
  /// Basic Sphere class to store sphere data and functions
  struct RaySphere {
    glm::vec3 position;
    float radius = 0.5f;
    
    int32_t material_index;
    
    /// This functiuon returns is day hitting the sphere
    /// - Parameters:
    ///   - ray: Ray comming from camera
    ///   - hit_distance: Last Hot distance
    bool Hit(const Ray& ray,
             float& hit_distance) const;
    
    /// This construct the sphere
    /// - Parameters:
    ///   - position: position of sphere center
    ///   - radius: rdius of sphere
    ///   - material_index: material index
    RaySphere(const glm::vec3& position, float radius, int32_t material_index);
    RaySphere() = default;
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(RaySphere);
  };
  
}
