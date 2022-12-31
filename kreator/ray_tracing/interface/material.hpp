//
//  material.hpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"
#include "hit.hpp"

namespace ray_tracing {
  
  struct Material {
    enum class Type : uint8_t {
      None, Metal, Lambertian
    };
    
    glm::vec3 albedo;
    Type type = Type::None;
    
    // metallic Property
    float fuzz; // Should range from 0 - 1
    
    /// This function scatters the ray based on the material property
    /// - Parameters:
    ///   - ray_in: current ray
    ///   - payload: hit payload
    ///   - attenuation: output color
    ///   - scattered_ray: output ray
    bool Scatter(const Ray& ray_in,
                 const HitPayload& payload,
                 glm::vec3& attenuation,
                 Ray& scattered_ray) const;
    
  private:
    /// This function scatters the ray For metals
    /// - Parameters:
    ///   - ray_in: current ray
    ///   - payload: hit payload
    ///   - attenuation: output color
    ///   - scattered_ray: output ray
    bool ScatterMatelic(const Ray& ray_in,
                        const HitPayload& payload,
                        glm::vec3& attenuation,
                        Ray& scattered_ray) const;
    /// This function scatters the ray For lambertian
    /// - Parameters:
    ///   - ray_in: current ray
    ///   - payload: hit payload
    ///   - attenuation: output color
    ///   - scattered_ray: output ray
    bool ScatterLambertian(const Ray& ray_in,
                           const HitPayload& payload,
                           glm::vec3& attenuation,
                           Ray& scattered_ray) const;
  };
  
}
