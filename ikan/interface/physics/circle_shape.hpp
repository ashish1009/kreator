//
//  circle_shape.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "shape.hpp"

namespace physics {
  
  /// A solid circle shape
  class CircleShape : public Shape {
  public:
    CircleShape();
    
    /// Implement Shape.
    Shape* Clone(BlockAllocator* allocator) const override;
    
    /// @see Shape::GetChildCount
    int32_t GetChildCount() const override;
    
    /// Implement Shape.
    bool TestPoint(const Transform& transform, const Vec2& p) const override;
    
    /// Implement Shape.
    /// @note because the circle is solid, rays that start inside do not hit because the normal is
    /// not defined.
    bool RayCast(RayCastOutput* output, const RayCastInput& input,
                 const Transform& transform, int32_t childIndex) const override;
    
    /// @see Shape::ComputeAABB
    void ComputeAABB(AABB* aabb, const Transform& transform, int32_t childIndex) const override;
    
    /// @see Shape::ComputeMass
    void ComputeMass(MassData* massData, float density) const override;
    
    /// Position
    Vec2 p;
  };
  
}
