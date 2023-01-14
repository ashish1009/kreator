//
//  collision.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  /// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
  struct RayCastInput {
    Vec2 p1, p2;
    float maxFraction;
  };
  
  /// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2 come from RayCastInput.
  struct RayCastOutput {
    Vec2 normal;
    float fraction;
  };
  
  /// An axis aligned bounding box.
  struct AABB {
    /// Verify that the bounds are sorted.
    bool IsValid() const;
    
    /// Get the center of the AABB.
    Vec2 GetCenter() const {
      return 0.5f * (lowerBound + upperBound);
    }
    
    /// Get the extents of the AABB (half-widths).
    Vec2 GetExtents() const {
      return 0.5f * (upperBound - lowerBound);
    }
    
    /// Get the perimeter length
    float GetPerimeter() const {
      float wx = upperBound.x - lowerBound.x;
      float wy = upperBound.y - lowerBound.y;
      return 2.0f * (wx + wy);
    }
    
    /// Combine an AABB into this one.
    void Combine(const AABB& aabb) {
      lowerBound = Min(lowerBound, aabb.lowerBound);
      upperBound = Max(upperBound, aabb.upperBound);
    }
    
    /// Combine two AABBs into this one.
    void Combine(const AABB& aabb1, const AABB& aab) {
      lowerBound = Min(aabb1.lowerBound, aab.lowerBound);
      upperBound = Max(aabb1.upperBound, aab.upperBound);
    }
    
    /// Does this aabb contain the provided AABB.
    bool Contains(const AABB& aabb) const {
      bool result = true;
      result = result && lowerBound.x <= aabb.lowerBound.x;
      result = result && lowerBound.y <= aabb.lowerBound.y;
      result = result && aabb.upperBound.x <= upperBound.x;
      result = result && aabb.upperBound.y <= upperBound.y;
      return result;
    }
    
    bool RayCast(RayCastOutput* output, const RayCastInput& input) const;
    
    Vec2 lowerBound;  ///< the lower vertex
    Vec2 upperBound;  ///< the upper vertex
  };

  inline bool TestOverlap(const AABB& a, const AABB& b) {
    Vec2 d1, d2;
    d1 = b.lowerBound - a.upperBound;
    d2 = a.lowerBound - b.upperBound;
    
    if (d1.x > 0.0f || d1.y > 0.0f)
      return false;
    
    if (d2.x > 0.0f || d2.y > 0.0f)
      return false;
    
    return true;
  }
  
}
