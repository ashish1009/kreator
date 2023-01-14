//
//  distance.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "distance.hpp"
#include "circle_shape.hpp"
#include "polygon_shape.hpp"
#include "chain_shape.hpp"
#include "edge_shape.hpp"

namespace physics {
  
  void DistanceProxy::Set(const Shape* shape, int32_t index) {
    switch (shape->GetType()) {
      case Shape::Type::Circle: {
        const CircleShape* circle = static_cast<const CircleShape*>(shape);
        vertices_ = &circle->p;
        count_ = 1;
        radius_ = circle->radius_;
      }
        break;
        
      case Shape::Type::Polygon: {
        const PolygonShape* polygon = static_cast<const PolygonShape*>(shape);
        vertices_ = polygon->vertices_;
        count_ = polygon->count_;
        radius_ = polygon->radius_;
      }
        break;
        
      case Shape::Type::Chain: {
        const ChainShape* chain = static_cast<const ChainShape*>(shape);
        IK_ASSERT(0 <= index && index < chain->count_);
        
        buffer_[0] = chain->vertices_[index];
        if (index + 1 < chain->count_) {
          buffer_[1] = chain->vertices_[index + 1];
        }
        else {
          buffer_[1] = chain->vertices_[0];
        }
        
        vertices_ = buffer_;
        count_ = 2;
        radius_ = chain->radius_;
      }
        break;
        
      case Shape::Type::Edge: {
        const EdgeShape* edge = static_cast<const EdgeShape*>(shape);
        vertices_ = &edge->vertex_1_;
        count_ = 2;
        radius_ = edge->radius_;
      }
        break;
        
      default:
        IK_ASSERT(false);
    }
  }
  
  void DistanceProxy::Set(const Vec2* vertices, int32_t count, float radius) {
    vertices_ = vertices;
    count_ = count;
    radius_ = radius;
  }

  
}
