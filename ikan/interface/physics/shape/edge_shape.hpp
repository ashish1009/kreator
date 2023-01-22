//
//  edge_shape.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#include "shape.hpp"

namespace physics {
  
  /// A line segment (edge) shape. These can be connected in chains or loops
  /// to other edge shapes. Edges created independently are two-sided and do
  /// no provide smooth movement across junctions.
  class EdgeShape : public Shape {
  public:
    EdgeShape();
    
    /// Set this as a part of a sequence. Vertex v0 precedes the edge and vertex v3
    /// follows. These extra vertices are used to provide smooth movement
    /// across junctions. This also makes the collision one-sided. The edge
    /// normal points to the right looking from v1 to v2.
    void SetOneSided(const Vec2& v0, const Vec2& v1,const Vec2& v2, const Vec2& v3);
    
    /// Set this as an isolated edge. Collision is two-sided.
    void SetTwoSided(const Vec2& v1, const Vec2& v2);
    
    /// Implement Shape.
    Shape* Clone(BlockAllocator* allocator) const override;
    
    /// @see Shape::GetChildCount
    int32_t GetChildCount() const override;
    
    /// @see Shape::TestPoint
    bool TestPoint(const Transform2D& transform, const Vec2& p) const override;
    
    /// Implement Shape.
    bool RayCast(RayCastOutput* output, const RayCastInput& input,
                 const Transform2D& transform, int32_t child_index) const override;
    
    /// @see Shape::ComputeAABB
    void ComputeAABB(AABB* aabb, const Transform2D& transform, int32_t child_index) const override;
    
    /// @see Shape::ComputeMass
    void ComputeMass(MassData* mass_data, float density) const override;
    
    /// These are the edge vertices
    Vec2 vertex_1_, vertex_2_;
    
    /// Optional adjacent vertices. These are used for smooth collision.
    Vec2 vertex_0_, vertex_3_;
    
    /// Uses m_vertex0 and m_vertex3 to create smooth collision.
    bool one_sided_;
  };
  
}
