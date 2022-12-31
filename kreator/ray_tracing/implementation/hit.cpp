//
//  hit.cpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#include "hit.hpp"

namespace ray_tracing {
  
  void HitPayload::SetFaceNormal(const Ray &ray, const glm::vec3 &outward_normal) {
    front_face = glm::dot(ray.direction, outward_normal) < 0;
    world_normal = front_face ? outward_normal : -outward_normal;
  }
  
}
