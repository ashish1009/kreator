//
//  physics_aabb.hpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#pragma once

namespace physics {
  
  /// Ray-cast input data. The ray extends from p1 to p1 + max_fraction * (p2 - p1).
  struct RayCastInput {
    Vec2 p1, p2;
    float max_fraction;
  };

  /// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2 come from RayCastInput.
  struct RayCastOutput {
    Vec2 normal;
    float fraction;
  };

  /// An axis aligned bounding box. for 2D
  struct AABB {
    /// This function verifies that the bounds are sorted.
    bool IsValid() const;
    /// This function returns the center of the AABB.
    Vec2 GetCenter() const;
    /// This function returns the extents of the AABB (half-widths).
    Vec2 GetExtents() const;
    /// This function returns the perimeter length
    float GetPerimeter() const;

    /// This function combines an AABB into this one.
    /// - Parameter aabb: aabb bounfing
    void Combine(const AABB& aabb);
    /// This function combines two AABBs into this one.
    /// - Parameters:
    ///   - aabb1: first aabb
    ///   - aabb2: second aabb
    void Combine(const AABB& aabb1, const AABB& aabb2);

    /// This function checks does this aabb contain the provided AABB.
    bool Contains(const AABB& aabb) const;
    
    /// This function Update the ray cast output if inpur ray hits the AABB
    /// - Important: From Real-time Collision Detection, p179.
    /// - Parameters:
    ///   - output: ray output data. This is updated
    ///   - input: ray input data
    bool RayCast(RayCastOutput* output, const RayCastInput& input) const;

    // Member Variables
    Vec2 lower_bound;
    Vec2 upper_bound;
  };
  
}

