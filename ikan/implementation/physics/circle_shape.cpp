//
//  circle_shape.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "circle_shape.hpp"
#include "block_allocator.hpp"

namespace physics {
  
  Shape* CircleShape::Clone(BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(CircleShape));
    CircleShape* clone = new (mem) CircleShape;
    *clone = *this;
    return clone;
  }
  
  int32_t CircleShape::GetChildCount() const {
    return 1;
  }
  
  bool CircleShape::TestPoint(const Transform& transform, const Vec2& p) const {
    Vec2 center = transform.p + Mul(transform.q, p);
    Vec2 d = p - center;
    return Dot(d, d) <= radius_ * radius_;
  }
  
  // Collision Detection in Interactive 3D Environments by Gino van den Bergen
  // From Section 3.1.2
  // x = s + a * r
  // norm(x) = radius
  bool CircleShape::RayCast(RayCastOutput* output, const RayCastInput& input,
                              const Transform& transform, int32_t childIndex) const {
    NOT_USED(childIndex);
    
    Vec2 position = transform.p + Mul(transform.q, position_);
    Vec2 s = input.p1 - position;
    float b = Dot(s, s) - radius_ * radius_;
    
    // Solve quadratic equation.
    Vec2 r = input.p2 - input.p1;
    float c =  Dot(s, r);
    float rr = Dot(r, r);
    float sigma = c * c - rr * b;
    
    // Check for negative discriminant and short segment.
    if (sigma < 0.0f || rr < FLT_EPSILON) {
      return false;
    }
    
    // Find the point of intersection of the line with the circle.
    float a = -(c + sqrt(sigma));
    
    // Is the intersection point on the segment?
    if (0.0f <= a && a <= input.maxFraction * rr) {
      a /= rr;
      output->fraction = a;
      output->normal = s + a * r;
      output->normal.Normalize();
      return true;
    }
    
    return false;
  }
  
  void CircleShape::ComputeAABB(AABB* aabb, const Transform& transform, int32_t childIndex) const {
    NOT_USED(childIndex);
    
    Vec2 p = transform.p + Mul(transform.q, this->position_);
    aabb->lowerBound.Set(p.x - radius_, p.y - radius_);
    aabb->upperBound.Set(p.x + radius_, p.y + radius_);
  }
  
  void CircleShape::ComputeMass(MassData* massData, float density) const {
    massData->mass = density * PI * radius_ * radius_;
    massData->center = position_;
    
    // inertia about the local origin
    massData->inertia = massData->mass * (0.5f * radius_ * radius_ + Dot(position_, position_));
  }

  inline CircleShape::CircleShape() {
    type_ = Shape::Type::Circle;
    radius_ = 0.0f;
    position_.SetZero();
  }
  

}
