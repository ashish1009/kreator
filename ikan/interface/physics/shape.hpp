//
//  shape.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "collision.hpp"

namespace physics {
  
  class BlockAllocator;
  
  /// This holds the mass data computed for a shape.
  struct MassData {
    /// The mass of the shape, usually in kilograms.
    float mass;
    
    /// The position of the shape's centroid relative to the shape's origin.
    Vec2 center;
    
    /// The rotational inertia of the shape about the local origin.
    float inertia;
  };
  
  /// A shape is used for collision detection. You can create a shape however you like.
  /// Shapes used for simulation in World are created automatically when a Fixture
  /// is created. Shapes may encapsulate a one or more child shapes.
  class Shape {
  public:
    enum class Type : uint8_t {
      Circle = 0,
      Edge = 1,
      Polygon = 2,
      Chain = 3,
      TypeCount = 4
    };

    virtual ~Shape() = default;
    
    /// Clone the concrete shape using the provided allocator.
    virtual Shape* Clone(BlockAllocator* allocator) const = 0;
    
    /// Get the type of this shape. You can use this to down cast to the concrete shape.
    /// - Returns the shape type.
    Type GetType() const;
    
    /// Get the number of child primitives.
    virtual int32_t GetChildCount() const = 0;
    
    /// Test a point for containment in this shape. This only works for convex shapes.
    /// - Parameters:
    ///   - xf: the shape world transform.
    ///   - p: a point in world coordinates.
    virtual bool TestPoint(const Transform& xf, const Vec2& p) const = 0;

    /// Cast a ray against a child shape.
    /// - Parameters:
    ///   - output: the ray-cast results.
    ///   - input: the ray-cast input parameters.
    ///   - transform: the transform to be applied to the shape.
    ///   - childIndex: the child shape index
    virtual bool RayCast(RayCastOutput* output, const RayCastInput& input,
                         const Transform& transform, int32_t childIndex) const = 0;

    /// Given a transform, compute the associated axis aligned bounding box for a child shape.
    /// - Parameters:
    ///   - aabb: returns the axis aligned box.
    ///   - xf: the world transform of the shape.
    ///   - childIndex: the child shape
    virtual void ComputeAABB(AABB* aabb, const Transform& xf, int32_t childIndex) const = 0;

    /// Compute the mass properties of this shape using its dimensions and density.
    /// The inertia tensor is computed about the local origin.
    /// - Parameters:
    ///   - massData: returns the mass data for this shape.
    ///   - density: the density in kilograms per meter squared.
    virtual void ComputeMass(MassData* massData, float density) const = 0;
    
    // ---------------------
    // Variables
    // ---------------------
    Type type_;
    /// Radius of a shape. For polygonal shapes this must be _polygonRadius. There is no support for /// making rounded polygons.
    float radius_;
  };
  
  inline Shape::Type Shape::GetType() const {
    return type_;
  }
  
}
