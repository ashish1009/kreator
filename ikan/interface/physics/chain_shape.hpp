//
//  chain_shape.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "shape.hpp"

namespace physics {

  class EdgeShape;
  
  /// A chain shape is a free form sequence of line segments.
  /// The chain has one-sided collision, with the surface normal pointing to the right of the edge.
  /// This provides a counter-clockwise winding like the polygon shape.
  /// Connectivity information is used to create smooth collisions.
  /// @warning the chain will not collide properly if there are self-intersections.
  class ChainShape : public Shape {
  public:
    ChainShape();
    
    /// The destructor frees the vertices using Free.
    ~ChainShape();
    
    /// Clear all data.
    void Clear();
    
    /// Create a loop. This automatically adjusts connectivity.
    /// @param vertices an array of vertices, these are copied
    /// @param count the vertex count
    void CreateLoop(const Vec2* vertices, int32_t count);
    
    /// Create a chain with ghost vertices to connect multiple chains together.
    /// @param vertices an array of vertices, these are copied
    /// @param count the vertex count
    /// @param prevVertex previous vertex from chain that connects to the start
    /// @param nextVertex next vertex from chain that connects to the end
    void CreateChain(const Vec2* vertices, int32_t count,
                     const Vec2& prevVertex, const Vec2& nextVertex);
    
    /// Implement Shape. Vertices are cloned using Alloc.
    Shape* Clone(BlockAllocator* allocator) const override;
    
    /// @see Shape::GetChildCount
    int32_t GetChildCount() const override;
    
    /// Get a child edge.
    void GetChildEdge(EdgeShape* edge, int32_t index) const;
    
    /// This always return false.
    /// @see Shape::TestPoint
    bool TestPoint(const Transform& transform, const Vec2& p) const override;
    
    /// Implement Shape.
    bool RayCast(RayCastOutput* output, const RayCastInput& input,
                 const Transform& transform, int32_t childIndex) const override;
    
    /// @see Shape::ComputeAABB
    void ComputeAABB(AABB* aabb, const Transform& transform, int32_t childIndex) const override;
    
    /// Chains have zero mass.
    /// @see Shape::ComputeMass
    void ComputeMass(MassData* massData, float density) const override;
    
    /// The vertices. Owned by this class.
    Vec2* vertices_;
    
    /// The vertex count.
    int32_t count_;
    
    Vec2 prev_vertex_, next_vertex_;
  };
  
}
