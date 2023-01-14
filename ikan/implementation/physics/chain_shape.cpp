//
//  chain_shape.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "chain_shape.hpp"
#include "block_allocator.hpp"
#include "edge_shape.hpp"

namespace physics {

  ChainShape::~ChainShape() {
    Clear();
  }
  
  void ChainShape::Clear() {
     FreeMem(vertices_);
    vertices_ = nullptr;
    count_ = 0;
  }
  
  void ChainShape::CreateLoop(const Vec2* vertices, int32_t count) {
    IK_ASSERT(vertices_ == nullptr && count_ == 0);
    IK_ASSERT(count >= 3);
    if (count < 3) {
      return;
    }
    
    for (int32_t i = 1; i < count; ++i) {
      Vec2 v1 = vertices[i-1];
      Vec2 v2 = vertices[i];
      // If the code crashes here, it means your vertices are too close together.
      IK_ASSERT(DistanceSquared(v1, v2) > LinearSlop * LinearSlop);
    }
    
    count_ = count + 1;
    vertices_ = (Vec2*)AllocMem(count_ * sizeof(Vec2));
    memcpy(vertices_, vertices, count * sizeof(Vec2));
    vertices_[count] = vertices_[0];
    prev_vertex_ = vertices_[count_ - 2];
    next_vertex_ = vertices_[1];
  }
  
  void ChainShape::CreateChain(const Vec2* vertices, int32_t count,  const Vec2& prevVertex, const Vec2& nextVertex) {
    IK_ASSERT(vertices_ == nullptr && count_ == 0);
    IK_ASSERT(count >= 2);
    for (int32_t i = 1; i < count; ++i) {
      // If the code crashes here, it means your vertices are too close together.
      IK_ASSERT(DistanceSquared(vertices[i-1], vertices[i]) > LinearSlop * LinearSlop);
    }
    
    count_ = count;
    vertices_ = (Vec2*)AllocMem(count * sizeof(Vec2));
    memcpy(vertices_, vertices, count_ * sizeof(Vec2));
    
    prev_vertex_ = prevVertex;
    next_vertex_ = nextVertex;
  }
  
  Shape* ChainShape::Clone(BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(ChainShape));
    ChainShape* clone = new (mem) ChainShape;
    clone->CreateChain(vertices_, count_, prev_vertex_, next_vertex_);
    return clone;
  }
  
  int32_t ChainShape::GetChildCount() const {
    // edge count = vertex count - 1
    return count_ - 1;
  }
  
  void ChainShape::GetChildEdge(EdgeShape* edge, int32_t index) const {
    IK_ASSERT(0 <= index && index < count_ - 1);
    edge->type_ = Shape::Type::Edge;
    edge->radius_ = radius_;
    
    edge->vertex_1_ = vertices_[index + 0];
    edge->vertex_2_ = vertices_[index + 1];
    edge->one_sided_ = true;
    
    if (index > 0) {
      edge->vertex_0_ = vertices_[index - 1];
    }
    else {
      edge->vertex_0_ = prev_vertex_;
    }
    
    if (index < count_ - 2) {
      edge->vertex_3_ = vertices_[index + 2];
    }
    else {
      edge->vertex_3_ = next_vertex_;
    }
  }
  
  bool ChainShape::TestPoint(const Transform& xf, const Vec2& p) const {
    NOT_USED(xf);
    NOT_USED(p);
    return false;
  }
  
  bool ChainShape::RayCast(RayCastOutput* output, const RayCastInput& input,
                             const Transform& xf, int32_t childIndex) const {
    IK_ASSERT(childIndex < count_);
    
    EdgeShape edgeShape;
    
    int32_t i1 = childIndex;
    int32_t i2 = childIndex + 1;
    if (i2 == count_) {
      i2 = 0;
    }
    
    edgeShape.vertex_1_ = vertices_[i1];
    edgeShape.vertex_3_ = vertices_[i2];
    
    return edgeShape.RayCast(output, input, xf, 0);
  }
  
  void ChainShape::ComputeAABB(AABB* aabb, const Transform& xf, int32_t childIndex) const {
    IK_ASSERT(childIndex < count_);
    
    int32_t i1 = childIndex;
    int32_t i2 = childIndex + 1;
    if (i2 == count_) {
      i2 = 0;
    }
    
    Vec2 v1 = Mul(xf, vertices_[i1]);
    Vec2 v2 = Mul(xf, vertices_[i2]);
    
    Vec2 lower = Min(v1, v2);
    Vec2 upper = Max(v1, v2);
    
    Vec2 r(radius_, radius_);
    aabb->lowerBound = lower - r;
    aabb->upperBound = upper + r;
  }
  
  void ChainShape::ComputeMass(MassData* massData, float density) const {
    NOT_USED(density);
    
    massData->mass = 0.0f;
    massData->center.SetZero();
    massData->inertia = 0.0f;
  }

  inline ChainShape::ChainShape() {
    type_ = Shape::Type::Chain;
    radius_ = PolygonRadius;
    vertices_ = nullptr;
    count_ = 0;
  }

  
}
