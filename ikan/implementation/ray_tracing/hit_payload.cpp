//
//  hit.cpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#include "hit_payload.hpp"

namespace ikan {
  
  void HitPayload::SetFaceNormal(const Ray &ray) {
    front_face = glm::dot(ray.direction, world_normal) >= 0;
    world_normal = front_face ? -world_normal : world_normal;
  }
  
}
