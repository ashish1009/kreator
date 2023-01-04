//
//  HitPayload.hpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"

namespace ikan {

  struct HitPayload {
    float hit_distance;
    glm::vec3 world_normal;
    glm::vec3 world_position;
    bool front_face = false;
    int32_t object_idx = -1;
    
    /// This function update the normal direction
    /// - Parameters:
    ///   - ray: ray of the camera
    void SetFaceNormal(const Ray& ray);
    
    HitPayload() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(HitPayload);
  };

}
