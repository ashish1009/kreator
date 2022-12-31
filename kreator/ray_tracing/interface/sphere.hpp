//
//  sphere.hpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "ray.hpp"
#include "hit.hpp"

namespace ray_tracing {
  
  /// Basic Sphere class to store sphere data and functions
  struct Sphere {
    glm::vec3 position;
    float radius = 0.5f;
    
    /// This functiuon returns is day hitting the sphere
    /// - Parameters:
    ///   - ray: Ray comming from camera
    ///   - near_plane: camera near plane
    ///   - far_plane: camera far plane
    ///   - payload: hit payload output
    bool Hit(const Ray& ray, float near_plane, float far_plane, HitPayload& payload);
    
    /// This construct the sphere
    /// - Parameters:
    ///   - position: position of sphere center
    ///   - radius: rdius of sphere
    Sphere(const glm::vec3& position, float radius);
    Sphere() = default;
  };
  
}
