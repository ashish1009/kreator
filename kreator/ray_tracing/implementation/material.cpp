//
//  materia.cpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#include "material.hpp"

namespace ray_tracing {
  
  bool NearZeroVec(const::glm::vec3 e) {
    // Return true if the vector is close to zero in all dimensions.
    const auto s = 1e-8;
    return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
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
      case Material::Type::Lambertian:
        return ScatterLambertian(ray_in, payload, attenuation, scattered_ray);
      default:
        IK_ASSERT(false, "invalid type");
    }
  }

  bool Material::ScatterMatelic(const Ray& ray_in,
                                const HitPayload& payload,
                                glm::vec3& attenuation,
                                Ray& scattered_ray) const {
    glm::vec3 reflected = glm::reflect(glm::normalize(ray_in.direction), payload.world_normal);
    scattered_ray = Ray(payload.world_position, reflected);
    attenuation = albedo;
    return (dot(scattered_ray.direction, payload.world_normal) > 0);
  }
  
  bool Material::ScatterLambertian(const Ray& ray_in,
                                   const HitPayload& payload,
                                   glm::vec3& attenuation,
                                   Ray& scattered_ray) const {
    auto scatter_direction = payload.world_normal + glm::normalize(ikan::Math::RandomVec3());
    
    // Catch degenerate scatter direction
    if (NearZeroVec(scatter_direction))
      scatter_direction = payload.world_normal;
    
    scattered_ray = Ray(payload.world_position, scatter_direction);
    attenuation = albedo;
    return true;
  }
  
}
