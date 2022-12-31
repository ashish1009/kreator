//
//  sphere.cpp
//  ray_tracing
//
//  Created by Ashish . on 31/12/22.
//

#include "sphere.hpp"

namespace ray_tracing {
  
  Sphere::Sphere(const glm::vec3& position, float radius, const glm::vec3& albedo)
  : position(position), radius(radius), albedo(albedo) { }
  
  bool Sphere::Hit(const Ray& ray, float near_plane, float far_plane, HitPayload& payload) {
    glm::vec3 origin = ray.origin - position;

    // float a = ray_direction.x * ray_direction + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
    float a = glm::dot(ray.direction, ray.direction);
    float half_b = glm::dot(origin, ray.direction);
    float c = glm::dot(origin, origin) - (radius * radius);

    // Discriminant
    // b^2 -4ac -> 4 * (b/2)^2 - 4 * ac -> 4 * ((b/2)^2 - ac)
    float discriminant = (half_b * half_b) - (a * c);

    // Find the nearest root that lies in the acceptable range.
    // -b +- sqrt(discriminant) / 2a
    float closest_t = (-half_b - glm::sqrt(discriminant)) / a;
    if (closest_t < near_plane || far_plane < closest_t) {
      closest_t = (-half_b + glm::sqrt(discriminant)) / a;
      if (closest_t < near_plane || far_plane < closest_t)
        return false;
    }
    
    // Hit the sphere
    payload.hit_distance = closest_t;
    payload.world_position = ray.At(closest_t);
    glm::vec3 outward_normal = (payload.world_position - position) / radius;
    payload.SetFaceNormal(ray, outward_normal);

    return true;
  }
  
}
