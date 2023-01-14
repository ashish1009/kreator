//
//  colloid_circle.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "collision.hpp"
#include "circle_shape.hpp"
#include "polygon_shape.hpp"

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
 
  void CollidePolygonAndCircle(Manifold* manifold,
                                 const PolygonShape* polygonA, const Transform& xfA,
                                 const CircleShape* circleB, const Transform& xfB) {
    manifold->point_count = 0;
    
    // Compute circle position in the frame of the polygon.
    Vec2 c = Mul(xfB, circleB->p);
    Vec2 cLocal = MulT(xfA, c);
    
    // Find the min separating edge.
    int32_t normalIndex = 0;
    float separation = -FLT_MAX;
    float radius = polygonA->radius_ + circleB->radius_;
    int32_t vertexCount = polygonA->count_;
    const Vec2* vertices = polygonA->vertices_;
    const Vec2* normals = polygonA->normals_;
    
    for (int32_t i = 0; i < vertexCount; ++i) {
      float s = Dot(normals[i], cLocal - vertices[i]);
      
      if (s > radius) {
        // Early out.
        return;
      }
      
      if (s > separation) {
        separation = s;
        normalIndex = i;
      }
    }
    
    // Vertices that subtend the incident face.
    int32_t vertIndex1 = normalIndex;
    int32_t vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
    Vec2 v1 = vertices[vertIndex1];
    Vec2 v2 = vertices[vertIndex2];
    
    // If the center is inside the polygon ...
    if (separation < FLT_EPSILON)
    {
      manifold->point_count = 1;
      manifold->type = Manifold::Type::FaceA;
      manifold->local_normal = normals[normalIndex];
      manifold->local_point = 0.5f * (v1 + v2);
      manifold->points[0].local_point = circleB->p;
      manifold->points[0].id.key = 0;
      return;
    }
    
    // Compute barycentric coordinates
    float u1 = Dot(cLocal - v1, v2 - v1);
    float u2 = Dot(cLocal - v2, v1 - v2);
    if (u1 <= 0.0f)
    {
      if (DistanceSquared(cLocal, v1) > radius * radius)
      {
        return;
      }
      
      manifold->point_count = 1;
      manifold->type = Manifold::Type::FaceA;
      manifold->local_normal = cLocal - v1;
      manifold->local_normal.Normalize();
      manifold->local_point = v1;
      manifold->points[0].local_point = circleB->p;
      manifold->points[0].id.key = 0;
    }
    else if (u2 <= 0.0f)
    {
      if (DistanceSquared(cLocal, v2) > radius * radius)
      {
        return;
      }
      
      manifold->point_count = 1;
      manifold->type = Manifold::Type::FaceA;
      manifold->local_normal = cLocal - v2;
      manifold->local_normal.Normalize();
      manifold->local_point = v2;
      manifold->points[0].local_point = circleB->p;
      manifold->points[0].id.key = 0;
    }
    else
    {
      Vec2 faceCenter = 0.5f * (v1 + v2);
      float s = Dot(cLocal - faceCenter, normals[vertIndex1]);
      if (s > radius)
      {
        return;
      }
      
      manifold->point_count = 1;
      manifold->type = Manifold::Type::FaceA;
      manifold->local_normal = normals[vertIndex1];
      manifold->local_point = faceCenter;
      manifold->points[0].local_point = circleB->p;
      manifold->points[0].id.key = 0;
    }
  }

}
