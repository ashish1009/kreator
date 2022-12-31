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
  
  bool Material::Scatter(const Ray &ray_in,
                         const HitPayload &payload,
                         glm::vec3 &attenuation,
                         Ray &scattered_ray) const {
    /*
     --------------------------------------------------------------------------
     NOTE: Not using dynamic dispatcher like virtual function to make better
     performance as this function will dispatch for each pixel (millions of times)
     --------------------------------------------------------------------------
     */
    switch (type) {
      case Material::Type::None:
        attenuation = albedo;
        scattered_ray = Ray(payload.world_position, -ray_in.direction);
        return true;
      case Material::Type::Metal:
        return ScatterMatelic(ray_in, payload, attenuation, scattered_ray);
      default:
        IK_ASSERT(false, "invalid type");
    }
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
