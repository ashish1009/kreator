//
//  circle_shape.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

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
    bool TestPoint(const Transform2D& transform, const Vec2& p) const override;
    
    /// Implement Shape.
    /// @note because the circle is solid, rays that start inside do not hit because the normal is
    /// not defined.
    bool RayCast(RayCastOutput* output, const RayCastInput& input,
                 const Transform2D& transform, int32_t child_index) const override;
    
    /// @see Shape::ComputeAABB
    void ComputeAABB(AABB* aabb, const Transform2D& transform, int32_t child_index) const override;
    
    /// @see Shape::ComputeMass
    void ComputeMass(MassData* mass_data, float density) const override;
    
    /// Position
    Vec2 position_;
  };
  
}
