//
//  hit.cpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#include "hit_payload.hpp"

namespace ray_tracing {
  
  HitPayload::HitPayload(const HitPayload& other)
  : hit_distance(other.hit_distance), world_normal(other.world_normal),
  world_position(other.world_position), front_face(other.front_face),
  object_idx(other.object_idx) {
    IK_CORE_TRACE(LogModule::HitPayload, "Copying Hit payload ...");
  }
  
  void HitPayload::SetFaceNormal(const Ray &ray) {
    front_face = glm::dot(ray.direction, world_normal) >= 0;
    world_normal = front_face ? -world_normal : world_normal;
  }
  
}
