//
//  polygon_shape.cpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#include "polygon_shape.hpp"
#include "block_allocator.hpp"

namespace physics {
  
  PolygonShape::PolygonShape() {
    type_ = Shape::Type::Polygon;
    radius_ = PolygonRadius;
    count_ = 0;
    centroid_.SetZero();
  }
  
  void PolygonShape::SetAsBox(float hx, float hy) {
    count_ = 4;
    vertices_[0].Set(-hx, -hy);
    vertices_[1].Set( hx, -hy);
    vertices_[2].Set( hx,  hy);
    vertices_[3].Set(-hx,  hy);
    normals_[0].Set(0.0f, -1.0f);
    normals_[1].Set(1.0f, 0.0f);
    normals_[2].Set(0.0f, 1.0f);
    normals_[3].Set(-1.0f, 0.0f);
    centroid_.SetZero();
  }
  
  void PolygonShape::SetAsBox(float hx, float hy, const Vec2& center, float angle) {
    SetAsBox(hx, hy);
    centroid_ = center;
    
    Transform2D xf;
    xf.p = center;
    xf.q.Set(angle);
    
    // Transform vertices and normals.
    for (int32_t i = 0; i < count_; ++i) {
      vertices_[i] = math::Mul(xf, vertices_[i]);
      normals_[i] = math::Mul(xf.q, normals_[i]);
    }
  }


}
