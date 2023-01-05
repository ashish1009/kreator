//
//  sphere.cpp
//  ikan
//
//  Created by Ashish . on 31/12/22.
//

#include "sphere.hpp"

namespace ikan {
  
  Sphere::Sphere(const Sphere& other)
  : position(other.position), radius(other.radius) {
//    IK_CORE_TRACE(LogModule::Sphere, "Copying Sphere ...");
  }
  
  Sphere::Sphere(const glm::vec3& position, float radius, int32_t material_index)
  : position(position), radius(radius), material_index(material_index) { }
  
  bool Sphere::Hit(const Ray& ray,
                   float& hit_distance) const {
    //       at^2       +       bt        +       c             = 0
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0
    // where,
    //    a : Ray Origin
    //    b : Direction of Ray
    //    r : Radius of Cirlce/Shphere
    //    t : Distance of point on ray from 'a'

    glm::vec3 origin = ray.origin - position;

    // float a = ray_direction.x * ray_direction + ray_direction.y * ray_direction.y + ray_direction.z * ray_direction.z;
    float a = glm::dot(ray.direction, ray.direction);
    float half_b = glm::dot(origin, ray.direction);
    float c = glm::dot(origin, origin) - (radius * radius);

    // Discriminant
    // b^2 -4ac
    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) {
      return false;
    }
    
    // Find the nearest root that lies in the acceptable range.
    // -b +- sqrt(discriminant) / 2a
    float closest_t = (-half_b - glm::sqrt(discriminant)) / a;

    if (closest_t < 0.001f || hit_distance < closest_t) {
      float other_t = (-half_b + glm::sqrt(discriminant)) / a;
      if (other_t < 0.001f || hit_distance < other_t)
        return false;
      hit_distance = other_t;
    } else {
      hit_distance = closest_t;
    }
    
    return true;
  }
  
}
