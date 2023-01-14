//
//  colloid_circle.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "collision.hpp"
#include "circle_shape.hpp"

namespace physics {
  
  void CollideCircles(Manifold* manifold,
                      const CircleShape* circleA, const Transform& xfA,
                      const CircleShape* circleB, const Transform& xfB) {
    manifold->point_count = 0;
    
    Vec2 pA = Mul(xfA, circleA->p);
    Vec2 pB = Mul(xfB, circleB->p);
    
    Vec2 d = pB - pA;
    float distSqr = Dot(d, d);
    float rA = circleA->radius_, rB = circleB->radius_;
    float radius = rA + rB;
    if (distSqr > radius * radius) {
      return;
    }
    
    manifold->type = Manifold::Type::Circles ;
    manifold->local_point = circleA->p;
    manifold->local_normal.SetZero();
    manifold->point_count = 1;
    
    manifold->points[0].local_point = circleB->p;
    manifold->points[0].id.key = 0;
  }
  
}
