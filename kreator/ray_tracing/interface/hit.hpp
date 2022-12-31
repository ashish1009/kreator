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
    bool front_face;
    int32_t object_idx = -1;
    
    /// This function update the normal direction
    /// - Parameters:
    ///   - ray: ray of the camera
    ///   - outward_normal: outward normal
    void SetFaceNormal(const Ray& ray, const glm::vec3& outward_normal);
  };

}
