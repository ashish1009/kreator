//
//  distance.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  class Shape;

  /// A distance proxy is used by the GJK algorithm.
  /// It encapsulates any shape.
  struct DistanceProxy {
    DistanceProxy() : vertices_(nullptr), count_(0), radius_(0.0f) {}
    
    /// Initialize the proxy using the given shape. The shape
    /// must remain in scope while the proxy is in use.
    void Set(const Shape* shape, int32_t index);
    
    /// Initialize the proxy using a vertex cloud and radius. The vertices
    /// must remain in scope while the proxy is in use.
    void Set(const Vec2* vertices, int32_t count, float radius);
    
    /// Get the supporting vertex index in the given direction.
    int32_t GetSupport(const Vec2& d) const;
    
    /// Get the supporting vertex in the given direction.
    const Vec2& GetSupportVertex(const Vec2& d) const;
    
    /// Get the vertex count.
    int32_t GetVertexCount() const;
    
    /// Get a vertex by index. Used by Distance.
    const Vec2& GetVertex(int32_t index) const;
    
    Vec2 buffer_[2];
    const Vec2* vertices_;
    int32_t count_;
    float radius_;
  };
  
  /// Input for Distance.
  /// You have to option to use the shape radii
  /// in the computation. Even
  struct DistanceInput {
    DistanceProxy proxyA;
    DistanceProxy proxyB;
    Transform transformA;
    Transform transformB;
    bool useRadii;
  };
  
}
