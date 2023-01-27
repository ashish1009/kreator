//
//  physics_aabb.cpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#include "collision.hpp"

namespace physics {
  
  using namespace math;
  
  bool AABB::IsValid() const {
    Vec2 d = upper_bound - lower_bound;
    bool valid = d.x >= 0.0f and d.y >= 0.0f;
    valid = valid and lower_bound.IsValid() and upper_bound.IsValid();
    return valid;
  }
  
  Vec2 AABB::GetCenter() const {
    return 0.5f * (lower_bound + upper_bound);
  }

  Vec2 AABB::GetExtents() const {
    return 0.5f * (upper_bound - lower_bound);
  }
  
  float AABB::GetPerimeter() const {
    float wx = upper_bound.x - lower_bound.x;
    float wy = upper_bound.y - lower_bound.y;
    return 2.0f * (wx + wy);
  }

  void AABB::Combine(const AABB& aabb) {
    lower_bound = Min(lower_bound, aabb.lower_bound);
    upper_bound = Max(upper_bound, aabb.upper_bound);
  }
  
  void AABB::Combine(const AABB& aabb1, const AABB& aab) {
    lower_bound = Min(aabb1.lower_bound, aab.lower_bound);
    upper_bound = Max(aabb1.upper_bound, aab.upper_bound);
  }

  bool AABB::Contains(const AABB& aabb) const {
    bool result = true;
    result = result and lower_bound.x <= aabb.lower_bound.x;
    result = result and lower_bound.y <= aabb.lower_bound.y;
    result = result and aabb.upper_bound.x <= upper_bound.x;
    result = result and aabb.upper_bound.y <= upper_bound.y;
    return result;
  }
  
  bool AABB::RayCast(RayCastOutput* output, const RayCastInput& input) const {
    float t_min = -FLT_MAX;
    float t_max = FLT_MAX;
    
    Vec2 p = input.p1;
    Vec2 d = input.p2 - input.p1;
    Vec2 abs_d = math::Abs(d);

    Vec2 normal;

    for (int32_t i = 0; i < 2; ++i) {
      if (abs_d(i) < Epsilon) {
        // Parallel.
        if (p(i) < lower_bound(i) or upper_bound(i) < p(i))
          return false;
      } else {
        float inv_d = 1.0f / d(i);
        float t1 = (lower_bound(i) - p(i)) * inv_d;
        float t2 = (upper_bound(i) - p(i)) * inv_d;

        // Sign of the normal vector.
        float s = -1.0f;

        if (t1 > t2) {
          math::Swap(t1, t2);
          s = 1.0f;
        }

        // Push the min up
        if (t1 > t_min) {
          normal.SetZero();
          normal(i) = s;
          t_min = t1;
        }

        // Pull the max down
        t_max = math::Min(t_max, t2);

        if (t_min > t_max) {
          return false;
        }
      }
    }

    // Does the ray start inside the box?
    // Does the ray intersect beyond the max fraction?
    if (t_min < 0.0f or input.max_fraction < t_min) {
      return false;
    }

    // Intersection.
    output->fraction = t_min;
    output->normal = normal;
    return true;
  }

}
