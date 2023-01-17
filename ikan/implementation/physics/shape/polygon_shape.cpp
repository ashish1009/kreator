//
//  polygon_shape.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "polygon_shape.hpp"
#include "block_allocator.hpp"

namespace physics {
  
  Shape* PolygonShape::Clone(BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(PolygonShape));
    PolygonShape* clone = new (mem) PolygonShape;
    *clone = *this;
    return clone;
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
    count_ = 4;
    vertices_[0].Set(-hx, -hy);
    vertices_[1].Set( hx, -hy);
    vertices_[2].Set( hx,  hy);
    vertices_[3].Set(-hx,  hy);
    normals_[0].Set(0.0f, -1.0f);
    normals_[1].Set(1.0f, 0.0f);
    normals_[2].Set(0.0f, 1.0f);
    normals_[3].Set(-1.0f, 0.0f);
    centroid_ = center;
    
    Transform xf;
    xf.p = center;
    xf.q.Set(angle);
    
    // Transform vertices and normals.
    for (int32_t i = 0; i < count_; ++i) {
      vertices_[i] = Mul(xf, vertices_[i]);
      normals_[i] = Mul(xf.q, normals_[i]);
    }
  }
  
  int32_t PolygonShape::GetChildCount() const {
    return 1;
  }
  
  static Vec2 ComputeCentroid(const Vec2* vs, int32_t count) {
    IK_ASSERT(count >= 3);
    
    Vec2 c(0.0f, 0.0f);
    float area = 0.0f;
    
    // Get a reference point for forming triangles.
    // Use the first vertex to reduce round-off errors.
    Vec2 s = vs[0];
    
    const float inv3 = 1.0f / 3.0f;
    
    for (int32_t i = 0; i < count; ++i) {
      // Triangle vertices.
      Vec2 p1 = vs[0] - s;
      Vec2 p2 = vs[i] - s;
      Vec2 p3 = i + 1 < count ? vs[i+1] - s : vs[0] - s;
      
      Vec2 e1 = p2 - p1;
      Vec2 e2 = p3 - p1;
      
      float D = Cross(e1, e2);
      
      float triangleArea = 0.5f * D;
      area += triangleArea;
      
      // Area weighted centroid
      c += triangleArea * inv3 * (p1 + p2 + p3);
    }
    
    // Centroid
    IK_ASSERT(area > FLT_EPSILON);
    c = (1.0f / area) * c + s;
    return c;
  }
  
  void PolygonShape::Set(const Vec2* vertices, int32_t count) {
    IK_ASSERT(3 <= count && count <= MaxPolygonVertices);
    if (count < 3) {
      SetAsBox(1.0f, 1.0f);
      return;
    }
    
    int32_t n = Min(count, MaxPolygonVertices);
    
    // Perform welding and copy vertices into local buffer.
    Vec2 ps[MaxPolygonVertices];
    int32_t tempCount = 0;
    for (int32_t i = 0; i < n; ++i) {
      Vec2 v = vertices[i];
      
      bool unique = true;
      for (int32_t j = 0; j < tempCount; ++j) {
        if (DistanceSquared(v, ps[j]) < ((0.5f * LinearSlop) * (0.5f * LinearSlop))) {
          unique = false;
          break;
        }
      }
      
      if (unique) {
        ps[tempCount++] = v;
      }
    }
    
    n = tempCount;
    if (n < 3) {
      // Polygon is degenerate.
      IK_ASSERT(false);
      SetAsBox(1.0f, 1.0f);
      return;
    }
    
    // Create the convex hull using the Gift wrapping algorithm
    // http://en.wikipedia.org/wiki/Gift_wrapping_algorithm
    
    // Find the right most point on the hull
    int32_t i0 = 0;
    float x0 = ps[0].x;
    for (int32_t i = 1; i < n; ++i) {
      float x = ps[i].x;
      if (x > x0 || (x == x0 && ps[i].y < ps[i0].y)) {
        i0 = i;
        x0 = x;
      }
    }
    
    int32_t hull[MaxPolygonVertices];
    int32_t m = 0;
    int32_t ih = i0;
    
    for (;;) {
      IK_ASSERT(m < MaxPolygonVertices);
      hull[m] = ih;
      
      int32_t ie = 0;
      for (int32_t j = 1; j < n; ++j) {
        if (ie == ih) {
          ie = j;
          continue;
        }
        
        Vec2 r = ps[ie] - ps[hull[m]];
        Vec2 v = ps[j] - ps[hull[m]];
        float c = Cross(r, v);
        if (c < 0.0f) {
          ie = j;
        }
        
        // Collinearity check
        if (c == 0.0f && v.LengthSquared() > r.LengthSquared()) {
          ie = j;
        }
      }
      
      ++m;
      ih = ie;
      
      if (ie == i0) {
        break;
      }
    }
    
    if (m < 3) {
      // Polygon is degenerate.
      IK_ASSERT(false);
      SetAsBox(1.0f, 1.0f);
      return;
    }
    
    count_ = m;
    
    // Copy vertices.
    for (int32_t i = 0; i < m; ++i) {
      vertices_[i] = ps[hull[i]];
    }
    
    // Compute normals. Ensure the edges have non-zero length.
    for (int32_t i = 0; i < m; ++i) {
      int32_t i1 = i;
      int32_t i2 = i + 1 < m ? i + 1 : 0;
      Vec2 edge = vertices_[i2] - vertices_[i1];
      IK_ASSERT(edge.LengthSquared() > FLT_EPSILON * FLT_EPSILON);
      normals_[i] = Cross(edge, 1.0f);
      normals_[i].Normalize();
    }
    
    // Compute the polygon centroid.
    centroid_ = ComputeCentroid(vertices_, m);
  }
  
  bool PolygonShape::TestPoint(const Transform& xf, const Vec2& p) const {
    Vec2 pLocal = MulT(xf.q, p - xf.p);
    
    for (int32_t i = 0; i < count_; ++i) {
      float dot = Dot(normals_[i], pLocal - vertices_[i]);
      if (dot > 0.0f) {
        return false;
      }
    }
    
    return true;
  }
  
  bool PolygonShape::RayCast(RayCastOutput* output, const RayCastInput& input,
                               const Transform& xf, int32_t childIndex) const {
    NOT_USED(childIndex);
    
    // Put the ray into the polygon's frame of reference.
    Vec2 p1 = MulT(xf.q, input.p1 - xf.p);
    Vec2 p2 = MulT(xf.q, input.p2 - xf.p);
    Vec2 d = p2 - p1;
    
    float lower = 0.0f, upper = input.maxFraction;
    
    int32_t index = -1;
    
    for (int32_t i = 0; i < count_; ++i) {
      // p = p1 + a * d
      // dot(normal, p - v) = 0
      // dot(normal, p1 - v) + a * dot(normal, d) = 0
      float numerator = Dot(normals_[i], vertices_[i] - p1);
      float denominator = Dot(normals_[i], d);
      
      if (denominator == 0.0f) {
        if (numerator < 0.0f) {
          return false;
        }
      }
      else {
        // Note: we want this predicate without division:
        // lower < numerator / denominator, where denominator < 0
        // Since denominator < 0, we have to flip the inequality:
        // lower < numerator / denominator <==> denominator * lower > numerator.
        if (denominator < 0.0f && numerator < lower * denominator) {
          // Increase lower.
          // The segment enters this half-space.
          lower = numerator / denominator;
          index = i;
        }
        else if (denominator > 0.0f && numerator < upper * denominator) {
          // Decrease upper.
          // The segment exits this half-space.
          upper = numerator / denominator;
        }
      }
      
      // The use of epsilon here causes the assert on lower to trip
      // in some cases. Apparently the use of epsilon was to make edge
      // shapes work, but now those are handled separately.
      //if (upper < lower - _epsilon)
      if (upper < lower) {
        return false;
      }
    }
    
    IK_ASSERT(0.0f <= lower && lower <= input.maxFraction);
    
    if (index >= 0) {
      output->fraction = lower;
      output->normal = Mul(xf.q, normals_[index]);
      return true;
    }
    
    return false;
  }
  
  void PolygonShape::ComputeAABB(AABB* aabb, const Transform& xf, int32_t childIndex) const {
    NOT_USED(childIndex);
    
    Vec2 lower = Mul(xf, vertices_[0]);
    Vec2 upper = lower;
    
    for (int32_t i = 1; i < count_; ++i)
    {
      Vec2 v = Mul(xf, vertices_[i]);
      lower = Min(lower, v);
      upper = Max(upper, v);
    }
    
    Vec2 r(radius_, radius_);
    aabb->lowerBound = lower - r;
    aabb->upperBound = upper + r;
  }
  
  void PolygonShape::ComputeMass(MassData* massData, float density) const {
    // Polygon mass, centroid, and inertia.
    // Let rho be the polygon density in mass per unit area.
    // Then:
    // mass = rho * int(dA)
    // centroid.x = (1/mass) * rho * int(x * dA)
    // centroid.y = (1/mass) * rho * int(y * dA)
    // I = rho * int((x*x + y*y) * dA)
    //
    // We can compute these integrals by summing all the integrals
    // for each triangle of the polygon. To evaluate the integral
    // for a single triangle, we make a change of variables to
    // the (u,v) coordinates of the triangle:
    // x = x0 + e1x * u + e2x * v
    // y = y0 + e1y * u + e2y * v
    // where 0 <= u && 0 <= v && u + v <= 1.
    //
    // We integrate u from [0,1-v] and then v from [0,1].
    // We also need to use the Jacobian of the transformation:
    // D = cross(e1, e2)
    //
    // Simplification: triangle centroid = (1/3) * (p1 + p2 + p3)
    //
    // The rest of the derivation is handled by computer algebra.
    
    IK_ASSERT(count_ >= 3);
    
    Vec2 center(0.0f, 0.0f);
    float area = 0.0f;
    float I = 0.0f;
    
    // Get a reference point for forming triangles.
    // Use the first vertex to reduce round-off errors.
    Vec2 s = vertices_[0];
    
    const float k_inv3 = 1.0f / 3.0f;
    
    for (int32_t i = 0; i < count_; ++i) {
      // Triangle vertices.
      Vec2 e1 = vertices_[i] - s;
      Vec2 e2 = i + 1 < count_ ? vertices_[i+1] - s : vertices_[0] - s;
      
      float D = Cross(e1, e2);
      
      float triangleArea = 0.5f * D;
      area += triangleArea;
      
      // Area weighted centroid
      center += triangleArea * k_inv3 * (e1 + e2);
      
      float ex1 = e1.x, ey1 = e1.y;
      float ex2 = e2.x, ey2 = e2.y;
      
      float intx2 = ex1*ex1 + ex2*ex1 + ex2*ex2;
      float inty2 = ey1*ey1 + ey2*ey1 + ey2*ey2;
      
      I += (0.25f * k_inv3 * D) * (intx2 + inty2);
    }
    
    // Total mass
    massData->mass = density * area;
    
    // Center of mass
    IK_ASSERT(area > FLT_EPSILON);
    center *= 1.0f / area;
    massData->center = center + s;
    
    // Inertia tensor relative to the local origin (point s).
    massData->inertia = density * I;
    
    // Shift to center of mass then to original body origin.
    massData->inertia += massData->mass * (Dot(massData->center, massData->center) - Dot(center, center));
  }
  
  bool PolygonShape::Validate() const {
    for (int32_t i = 0; i < count_; ++i) {
      int32_t i1 = i;
      int32_t i2 = i < count_ - 1 ? i1 + 1 : 0;
      Vec2 p = vertices_[i1];
      Vec2 e = vertices_[i2] - p;
      
      for (int32_t j = 0; j < count_; ++j) {
        if (j == i1 || j == i2) {
          continue;
        }
        
        Vec2 v = vertices_[j] - p;
        float c = Cross(e, v);
        if (c < 0.0f) {
          return false;
        }
      }
    }
    
    return true;
  }

  
  PolygonShape::PolygonShape() {
    type_ = Shape::Type::Polygon;
    radius_ = PolygonRadius;
    count_ = 0;
    centroid_.SetZero();
  }
  
}
