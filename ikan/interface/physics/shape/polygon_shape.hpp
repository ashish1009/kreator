//
//  polygon_shape.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#pragma once

#include "shape.hpp"

namespace physics {
  
  /// A solid convex polygon. It is assumed that the interior of the polygon is to the left of each edge.
  /// Polygons have a maximum number of vertices equal to MaxPolygonVertices. In most cases
  /// you should not need many vertices for a convex polygon.
  class PolygonShape : public Shape {
  public:
    PolygonShape();
    
    /// Implement Shape.
    Shape* Clone(BlockAllocator* allocator) const override {
      IK_ASSERT(false);
    }
    
    /// @see Shape::GetChildCount
    int32_t GetChildCount() const override {
      IK_ASSERT(false);
    }
    
    /// Create a convex hull from the given array of local points. The count must be in the range [3, MaxPolygonVertices].
    /// - Warning: the points may be re-ordered, even if they form a convex polygon
    /// - Warning: collinear points are handled but not removed. Collinear points may lead to poor stacking behavior.
    void Set(const Vec2* points, int32_t count);
    
    /// Build vertices to represent an axis-aligned box centered on the local origin.
    /// - Parameters:
    ///   - hx: the half-width.
    ///   - hy: the half-height.
    void SetAsBox(float hx, float hy);
    
    /// Build vertices to represent an oriented box.
    /// - Parameters:
    ///   - hx: the half-width.
    ///   - hy: the half-height.
    ///   - center: the center of the box in local coordinates.
    ///   - angle: the rotation of the box in local coordinates.
    void SetAsBox(float hx, float hy, const Vec2& center, float angle);
    
    /// @see Shape::TestPoint
    bool TestPoint(const Transform2D& transform, const Vec2& p) const override {
      IK_ASSERT(false);
    }
    
    /// Implement Shape.
    /// - Note: because the polygon is solid, rays that start inside do not hit because the normal is
    /// not defined.
    bool RayCast(RayCastOutput* output, const RayCastInput& input,
                 const Transform2D& transform, int32_t child_index) const override {
      IK_ASSERT(false);
    }
    
    /// @see Shape::ComputeAABB
    void ComputeAABB(AABB* aabb, const Transform2D& transform, int32_t child_index) const override {
      IK_ASSERT(false);
    }
    
    /// @see Shape::ComputeMass
    void ComputeMass(MassData* mass_data, float density) const override {
      IK_ASSERT(false);
    }
    
    /// Validate convexity. This is a very time consuming operation.
    /// - Returns: true if valid
    bool Validate() const;
    
    Vec2 centroid_;
    Vec2 vertices_[MaxPolygonVertices];
    Vec2 normals_[MaxPolygonVertices];
    int32_t count_;
  };
  
}
