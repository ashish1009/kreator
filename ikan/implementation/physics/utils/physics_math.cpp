//
//  physics_math.cpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#include "physics_math.hpp"

namespace physics {
  
  namespace math {
    
    bool IsValid(float x) {
      return isfinite(x);
    }
    float Dot(const Vec3& a, const Vec3& b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    Vec3 Cross(const Vec3& a, const Vec3& b) {
      return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    float Dot(const Vec2& a, const Vec2& b) {
      return a.x * b.x + a.y * b.y;
    }
    float Cross(const Vec2& a, const Vec2& b) {
      return a.x * b.y - a.y * b.x;
    }
    Vec2 Cross(float s, const Vec2& a) {
      return Vec2(-s * a.y, s * a.x);
    }

    Vec2 Mul(const Rot& q, const Vec2& v) {
      return Vec2(q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y);
    }
    Vec2 Mul(const Transform2D& T, const Vec2& v) {
      float x = (T.q.c * v.x - T.q.s * v.y) + T.p.x;
      float y = (T.q.s * v.x + T.q.c * v.y) + T.p.y;
      
      return Vec2(x, y);
    }

    Vec2 operator + (const Vec2& a, const Vec2& b) {
      return Vec2(a.x + b.x, a.y + b.y);
    }
    
    Vec2 operator - (const Vec2& a, const Vec2& b) {
      return Vec2(a.x - b.x, a.y - b.y);
    }
    
    Vec2 operator * (float s, const Vec2& a) {
      return Vec2(s * a.x, s * a.y);
    }

    Vec2 Min(const Vec2& a, const Vec2& b) {
      return Vec2(Min(a.x, b.x), Min(a.y, b.y));
    }
    Vec2 Max(const Vec2& a, const Vec2& b) {
      return Vec2(Max(a.x, b.x), Max(a.y, b.y));
    }
    
    Vec2 Abs(const Vec2& a) {
      return Vec2(Abs(a.x), Abs(a.y));
    }
    
    Mat22 Abs(const Mat22& A) {
      return Mat22(Abs(A.ex), Abs(A.ey));
    }
    
    bool operator == (const Vec2& a, const Vec2& b) {
      return a.x == b.x && a.y == b.y;
    }
    
    bool operator != (const Vec2& a, const Vec2& b) {
      return a.x != b.x || a.y != b.y;
    }

  }
  
  using namespace math;
  
  const Vec2 Vec2_zero(0.0f, 0.0f);
  
  Vec2::Vec2(float x_input, float y_input) : x(x_input), y(y_input) {}
  
  void Vec2::SetZero()  {
    x = 0.0f;
    y = 0.0f;
  }
  
  void Vec2::Set(float x_input, float y_input) {
    x = x_input;
    y = y_input;
  }
  
  float Vec2::Normalize() {
    float length = Length();
    if (length < Epsilon) {
      return 0.0f;
    }
    
    float inv_length = 1.0f / length;
    x *= inv_length;
    y *= inv_length;
    
    return length;
  }

  float Vec2::Length() const {
    return sqrtf(x * x + y * y);
  }
  
  float Vec2::LengthSquared() const {
    return x * x + y * y;
  }
  
  Vec2 Vec2::operator -() const {
    Vec2 v;
    v.Set(-x, -y);
    return v;
  }
  
  float Vec2::operator () (int32_t i) const {
    PHYSICS_ASSERT(i < 2 and i >= 0);
    return (&x)[i];
  }

  float& Vec2::operator () (int32_t i) {
    PHYSICS_ASSERT(i < 2 and i >= 0);
    return (&x)[i];
  }

  void Vec2::operator += (const Vec2& v) {
    x += v.x; y += v.y;
  }
  
  void Vec2::operator -= (const Vec2& v) {
    x -= v.x; y -= v.y;
  }
  
  void Vec2::operator *= (float a) {
    x *= a; y *= a;
  }

  void Vec2::operator /= (float a) {
    x /= a; y /= a;
  }

  bool Vec2::IsValid() const {
    return math::IsValid(x) and math::IsValid(y);
  }

  Vec2 Vec2::Skew() const {
    return Vec2(-y, x);
  }
  
  void Vec2::Log() const {
    PHYSICS_TRACE("  Value of Vector 2 :");
    PHYSICS_TRACE("    x : {0}", x);
    PHYSICS_TRACE("    y : {0}", y);
  }
  
  Vec3::Vec3(float x_input, float y_input, float z_input)
  : x(x_input), y(y_input), z(z_input) {}
  
  void Vec3::SetZero()  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }
  
  void Vec3::Set(float x_input, float y_input, float z_input) {
    x = x_input;
    y = y_input;
    z = z_input;
  }
  
  float Vec3::Normalize() {
    float length = Length();
    if (length < Epsilon) {
      return 0.0f;
    }
    
    float inv_length = 1.0f / length;
    x *= inv_length;
    y *= inv_length;
    z *= inv_length;
    
    return length;
  }
  
  float Vec3::Length() const {
    return sqrtf(x * x + y * y + z * z);
  }
  
  float Vec3::LengthSquared() const {
    return x * x + y * y;
  }
  
  Vec3 Vec3::operator -() const {
    Vec3 v;
    v.Set(-x, -y, -z);
    return v;
  }
  
  float Vec3::operator () (int32_t i) const {
    PHYSICS_ASSERT(i < 3 and i >= 0);
    return (&x)[i];
  }
  
  float& Vec3::operator () (int32_t i) {
    PHYSICS_ASSERT(i < 3 and i >= 0);
    return (&x)[i];
  }
  
  void Vec3::operator += (const Vec3& v) {
    x += v.x; y += v.y; z += v.z;
  }
  
  void Vec3::operator -= (const Vec3& v) {
    x -= v.x; y -= v.y; z -= v.z;
  }
  
  void Vec3::operator *= (float a) {
    x *= a; y *= a; z *= a;
  }
  
  void Vec3::operator /= (float a) {
    x /= a; y /= a; z /= a;
  }
  
  bool Vec3::IsValid() const {
    return math::IsValid(x) and math::IsValid(y) and math::IsValid(z);
  }
  
  void Vec3::Log() const {
    PHYSICS_TRACE("  Value of Vector 3 :");
    PHYSICS_TRACE("    x : {0}", x);
    PHYSICS_TRACE("    y : {0}", y);
    PHYSICS_TRACE("    z : {0}", z);
  }

  Mat22::Mat22(const Vec2& c1, const Vec2& c2) : ex(c1), ey(c2) {}
  
  Mat22::Mat22(float a11, float a12, float a21, float a22) {
    ex.x = a11; ex.y = a21;
    ey.x = a12; ey.y = a22;
  }

  void Mat22::Set(const Vec2& c1, const Vec2& c2) {
    ex = c1;
    ey = c2;
  }
  
  void Mat22::SetIdentity() {
    ex.x = 1.0f; ey.x = 0.0f;
    ex.y = 0.0f; ey.y = 1.0f;
  }
  
  void Mat22::SetZero() {
    ex.x = 0.0f; ey.x = 0.0f;
    ex.y = 0.0f; ey.y = 0.0f;
  }

  Mat22 Mat22::GetInverse() const {
    float a = ex.x, b = ey.x, c = ex.y, d = ey.y;
    float det = a * d - b * c;
    if (det != 0.0f) {
      det = 1.0f / det;
    }

    Mat22 B;
    B.ex.x =  det * d;  B.ey.x = -det * b;
    B.ex.y = -det * c;  B.ey.y =  det * a;
    return B;
  }

  Vec2 Mat22::Solve(const Vec2& b) const {
    float a11 = ex.x, a12 = ey.x, a21 = ex.y, a22 = ey.y;
    float det = a11 * a22 - a12 * a21;
    if (det != 0.0f) {
      det = 1.0f / det;
    }
    
    Vec2 x;
    x.x = det * (a22 * b.x - a12 * b.y);
    x.y = det * (a11 * b.y - a21 * b.x);
    return x;
  }
  
  void Mat22::Log() const {
    PHYSICS_TRACE("  Value of Matrix 2x2 :");
    PHYSICS_TRACE("    | {0} {1} |", ex.x, ey.x);
    PHYSICS_TRACE("    | {0} {1} |", ex.y, ey.y);
  }

  Mat33::Mat33(const Vec3& c1, const Vec3& c2, const Vec3& c3) : ex(c1), ey(c2), ez(c3) {}
  
  void Mat33::Set(const Vec3& c1, const Vec3& c2, const Vec3& c3) {
    ex = c1;
    ey = c2;
    ez = c3;
  }
  
  void Mat33::SetIdentity() {
    ex.x = 1.0f; ey.x = 0.0f; ez.x = 0.0f;
    ex.y = 0.0f; ey.y = 1.0f; ez.y = 0.0f;
    ex.z = 0.0f; ey.z = 0.0f; ez.z = 1.0f;
  }
  
  void Mat33::SetZero() {
    ex.SetZero();
    ey.SetZero();
    ez.SetZero();
  }

  void Mat33::GetInverse22(Mat33* M) const {
    float a = ex.x, b = ey.x, c = ex.y, d = ey.y;
    float det = a * d - b * c;
    if (det != 0.0f) {
      det = 1.0f / det;
    }
    
    M->ex.x =  det * d;  M->ey.x = -det * b; M->ex.z = 0.0f;
    M->ex.y = -det * c;  M->ey.y =  det * a; M->ey.z = 0.0f;
    M->ez.x = 0.0f; M->ez.y = 0.0f; M->ez.z = 0.0f;
  }
  
  void Mat33::GetSymInverse33(Mat33* M) const {
    float det = math::Dot(ex, math::Cross(ey, ez));
    if (det != 0.0f) {
      det = 1.0f / det;
    }
    
    float a11 = ex.x, a12 = ey.x, a13 = ez.x;
    float a22 = ey.y, a23 = ez.y;
    float a33 = ez.z;
    
    M->ex.x = det * (a22 * a33 - a23 * a23);
    M->ex.y = det * (a13 * a23 - a12 * a33);
    M->ex.z = det * (a12 * a23 - a13 * a22);
    
    M->ey.x = M->ex.y;
    M->ey.y = det * (a11 * a33 - a13 * a13);
    M->ey.z = det * (a13 * a12 - a11 * a23);
    
    M->ez.x = M->ex.z;
    M->ez.y = M->ey.z;
    M->ez.z = det * (a11 * a22 - a12 * a12);
  }

  Vec3 Mat33::Solve33(const Vec3& b) const {
    float det = math::Dot(ex, math::Cross(ey, ez));
    if (det != 0.0f) {
      det = 1.0f / det;
    }
    
    Vec3 x;
    x.x = det * math::Dot(b, math::Cross(ey, ez));
    x.y = det * math::Dot(ex, math::Cross(b, ez));
    x.z = det * math::Dot(ex, math::Cross(ey, b));
    return x;
  }
  
  Vec2 Mat33::Solve22(const Vec2& b) const {
    float a11 = ex.x, a12 = ey.x, a21 = ex.y, a22 = ey.y;
    float det = a11 * a22 - a12 * a21;
    if (det != 0.0f) {
      det = 1.0f / det;
    }
    
    Vec2 x;
    x.x = det * (a22 * b.x - a12 * b.y);
    x.y = det * (a11 * b.y - a21 * b.x);
    return x;
  }

  void Mat33::Log() const {
    PHYSICS_TRACE("  Value of Matrix 3x3 :");
    PHYSICS_TRACE("    | {0} {1} {2} |", ex.x, ey.x, ez.x);
    PHYSICS_TRACE("    | {0} {1} {2} |", ex.y, ey.y, ez.y);
    PHYSICS_TRACE("    | {0} {1} {2} |", ex.z, ey.z, ez.z);
  }
  
  Rot::Rot(float angle) : s(sinf(angle)), c(cosf(angle)) {}
  
  void Rot::Set(float angle) {
    s = sinf(angle);
    c = cosf(angle);
  }
  void Rot::SetIdentity() {
    s = 0.0f;
    c = 1.0f;
  }
  float Rot::GetAngle() const {
    return atan2f(s, c);
  }
  Vec2 Rot::GetXAxis() const {
    return Vec2(c, s);
  }
  Vec2 Rot::GetYAxis() const {
    return Vec2(-s, c);
  }

  void Rot::Log() const {
    PHYSICS_TRACE("  Rotation Data");
    PHYSICS_TRACE("    Radian : {0}", GetAngle());
    PHYSICS_TRACE("    Sin    : {0}", s);
    PHYSICS_TRACE("    Cos    : {0}", c);
  }
  
  Transform2D::Transform2D(const Vec2& position, const Rot& rotation) : p(position), q(rotation) {}
  
  void Transform2D::SetIdentity() {
    p.SetZero();
    q.SetIdentity();
  }
  
  void Transform2D::Set(const Vec2& position, float angle) {
    p = position;
    q.Set(angle);
  }
  
  void Transform2D::Log() const {
    PHYSICS_TRACE("  Transform Data");
    p.Log();
    q.Log();
  }
  
  void Sweep::GetTransform(Transform2D* xf, float beta) const {
    xf->p = (1.0f - beta) * c0 + beta * c;
    float angle = (1.0f - beta) * a0 + beta * a;
    xf->q.Set(angle);
    
    // Shift to origin
    xf->p -= Mul(xf->q, local_center);
  }
  
  void Sweep::Advance(float alpha) {
    PHYSICS_ASSERT(alpha0 < 1.0f);
    float beta = (alpha - alpha0) / (1.0f - alpha0);
    c0 += beta * (c - c0);
    a0 += beta * (a - a0);
    alpha0 = alpha;
  }

  void Sweep::Normalize() {
    float twoPi = 2.0f * PI;
    float d =  twoPi * floorf(a0 / twoPi);
    a0 -= d;
    a -= d;
  }

}
