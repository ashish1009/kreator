//
//  material.hpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"
#include "hit_payload.hpp"

namespace ikan {
  
  struct RayMaterial {
    enum class Type : uint8_t {
      None, Metal, Lambertian, Dielectric
    };
    
    glm::vec3 albedo;
    Type type = Type::None;
    
    // metallic Property
    float fuzz = 0.1; // Should range from 0 - 1
    
    // dielectric prop
    float refractive_index = 0.1;
    
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
    
    RayMaterial() =default;
    DELETE_COPY_MOVE_CONSTRUCTORS(RayMaterial);
    
  private:
    /// This function calculate the reflecatance for the dielectric
    /// - Parameters:
    ///   - cosine: cos value
    float Reflectance(float cosine) const;

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
    /// This function scatters the ray For Dielectric
    /// - Parameters:
    ///   - ray_in: current ray
    ///   - payload: hit payload
    ///   - attenuation: output color
    ///   - scattered_ray: output ray
    bool ScatterDielectric(const Ray& ray_in,
                           const HitPayload& payload,
                           glm::vec3& attenuation,
                           Ray& scattered_ray) const;
  };
  
}
