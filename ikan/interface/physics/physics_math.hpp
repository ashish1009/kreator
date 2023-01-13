//
//  physics_math.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  /// A 2D column vector.
  struct Vec2 {
    /// Default constructor does nothing (for performance).
    Vec2() {}
    
    /// Construct using coordinates.
    Vec2(float xIn, float yIn) : x(xIn), y(yIn) {}
    
    /// Set this vector to all zeros.
    void SetZero() { x = 0.0f; y = 0.0f; }
    
    /// Set this vector to some specified coordinates.
    void Set(float x_, float y_) { x = x_; y = y_; }
    
    /// Negate this vector.
    Vec2 operator -() const { Vec2 v; v.Set(-x, -y); return v; }
    
    /// Read from and indexed element.
    float operator () (int32_t i) const { return (&x)[i]; }
    
    /// Write to an indexed element.
    float& operator () (int32_t i) { return (&x)[i]; }
    
    /// Add a vector to this vector.
    void operator += (const Vec2& v) { x += v.x; y += v.y; }
    
    /// Subtract a vector from this vector.
    void operator -= (const Vec2& v) { x -= v.x; y -= v.y; }
    
    /// Multiply this vector by a scalar.
    void operator *= (float a) { x *= a; y *= a; }
    
    /// Get the length of this vector (the norm).
    float Length() const { return sqrtf(x * x + y * y); }
    
    /// Get the length squared. For performance, use this instead of
    /// b2Vec2::Length (if possible).
    float LengthSquared() const { return x * x + y * y; }
    
    /// Convert this vector into a unit vector. Returns the length.
    float Normalize() {
      float length = Length();
      if (length < FLT_EPSILON) {
        return 0.0f;
      }
      float invLength = 1.0f / length;
      x *= invLength;
      y *= invLength;
      
      return length;
    }
    
    /// Does this vector contain finite coordinates?
    bool IsValid() const { return isfinite(x) && isfinite(y); }
    
    /// Get the skew vector such that dot(skew_vec, other) == cross(vec, other)
    Vec2 Skew() const { return Vec2(-y, x); }
    
    float x, y;
  };

  
  /// Rotation
  struct Rot {
    Rot() = default;
    
    /// Initialize from an angle in radians
    explicit Rot(float angle) {
      /// TODO_ERIN optimize
      s = sinf(angle);
      c = cosf(angle);
    }
    
    /// Set using an angle in radians.
    void Set(float angle) {
      /// TODO_ERIN optimize
      s = sinf(angle);
      c = cosf(angle);
    }
    
    /// Set to the identity rotation
    void SetIdentity() {
      s = 0.0f;
      c = 1.0f;
    }
    
    /// Get the angle in radians
    float GetAngle() const { return atan2f(s, c); }
    /// Get the x-axis
    Vec2 GetXAxis() const { return Vec2(c, s); }
    /// Get the u-axis
    Vec2 GetYAxis() const { return Vec2(-s, c); }
    
    /// Sine and cosine
    float s, c;
  };

  /// A transform contains translation and rotation. It is used to represent the position and orientation of rigid frames.
  struct Transform {
    /// The default constructor does nothing.
    Transform() {}
    /// Initialize using a position vector and a rotation.
    Transform(const Vec2& position, const Rot& rotation) : p(position), q(rotation) {}
    
    /// Set this to the identity transform.
    void SetIdentity() {
      p = { 0, 0 };
      q.SetIdentity();
    }
    
    /// Set this based on the position and angle.
    void Set(const Vec2& position, float angle) {
      p = position;
      q.Set(angle);
    }
    
    Vec2 p;
    Rot q;
  };

}
