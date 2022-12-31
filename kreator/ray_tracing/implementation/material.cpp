//
//  materia.cpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#include "material.hpp"

namespace ray_tracing {
  
  glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
    return v - 2*dot(v,n)*n;
  }

  bool Material::ScatterMatelic(const Ray& ray_in,
                                const HitPayload& payload,
                                glm::vec3& attenuation,
                                Ray& scattered_ray) const {
    glm::vec3 reflected = reflect(glm::normalize(ray_in.direction), payload.world_normal);
    scattered_ray = Ray(payload.world_position, reflected);
    attenuation = albedo;
    return (dot(scattered_ray.direction, payload.world_normal) > 0);
  }
  
}
