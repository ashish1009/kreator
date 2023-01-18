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
  }
  
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
    PHYSICS_ASSERT(i < 2 and i >= 0, "Invalid Index");
    return (&x)[i];
  }

  float& Vec2::operator () (int32_t i) {
    PHYSICS_ASSERT(i < 2 and i >= 0, "Invalid Index");
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
    PHYSICS_ASSERT(i < 3 and i >= 0, "Invalid Index");
    return (&x)[i];
  }
  
  float& Vec3::operator () (int32_t i) {
    PHYSICS_ASSERT(i < 3 and i >= 0, "Invalid Index");
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
  
}
