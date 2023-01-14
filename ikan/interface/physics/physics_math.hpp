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
    Vec2() = default;
    
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
    /// Vec2::Length (if possible).
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
  
  /// A 2D column vector with 3 elements.
  struct Vec3 {
    /// Default constructor does nothing (for performance).
    Vec3() = default;
    
    /// Construct using coordinates.
    Vec3(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {}
    
    /// Set this vector to all zeros.
    void SetZero() { x = 0.0f; y = 0.0f; z = 0.0f; }
    
    /// Set this vector to some specified coordinates.
    void Set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
    
    /// Negate this vector.
    Vec3 operator -() const { Vec3 v; v.Set(-x, -y, -z); return v; }

    /// Add a vector to this vector.
    void operator += (const Vec3& v) { x += v.x; y += v.y; z += v.z; }
    
    /// Subtract a vector from this vector.
    void operator -= (const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; }
    
    /// Multiply this vector by a scalar.
    void operator *= (float s) { x *= s; y *= s; z *= s; }
    
    float x, y, z;
  };

  /// A 2-by-2 matrix. Stored in column-major order.
  struct Mat22 {
    /// The default constructor does nothing (for performance).
    Mat22() = default;
    
    /// Construct this matrix using columns.
    Mat22(const Vec2& c1, const Vec2& c2) {
      ex = c1;
      ey = c2;
    }
    
    /// Construct this matrix using scalars.
    Mat22(float a11, float a12, float a21, float a22) {
      ex.x = a11; ex.y = a21;
      ey.x = a12; ey.y = a22;
    }
    
    /// Initialize this matrix using columns.
    void Set(const Vec2& c1, const Vec2& c2) {
      ex = c1;
      ey = c2;
    }
    
    /// Set this to the identity matrix.
    void SetIdentity() {
      ex.x = 1.0f; ey.x = 0.0f;
      ex.y = 0.0f; ey.y = 1.0f;
    }
    
    /// Set this matrix to all zeros.
    void SetZero() {
      ex.x = 0.0f; ey.x = 0.0f;
      ex.y = 0.0f; ey.y = 0.0f;
    }
    
    Mat22 GetInverse() const {
      float a = ex.x, b = ey.x, c = ex.y, d = ey.y;
      Mat22 B;
      float det = a * d - b * c;
      if (det != 0.0f) {
        det = 1.0f / det;
      }
      B.ex.x =  det * d;  B.ey.x = -det * b;
      B.ex.y = -det * c;  B.ey.y =  det * a;
      return B;
    }
    
    /// Solve A * x = b, where b is a column vector. This is more efficient
    /// than computing the inverse in one-shot cases.
    Vec2 Solve(const Vec2& b) const {
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
    
    Vec2 ex, ey;
  };
  
  /// A 3-by-3 matrix. Stored in column-major order.
  struct Mat33 {
    /// The default constructor does nothing (for performance).
    Mat33() = default;
    
    /// Construct this matrix using columns.
    Mat33(const Vec3& c1, const Vec3& c2, const Vec3& c3) {
      ex = c1;
      ey = c2;
      ez = c3;
    }
    
    /// Set this matrix to all zeros.
    void SetZero() {
      ex.SetZero();
      ey.SetZero();
      ez.SetZero();
    }
    
    /// Solve A * x = b, where b is a column vector. This is more efficient than computing the inverse in one-shot cases.
    Vec3 Solve33(const Vec3& b) const;
    
    /// Solve A * x = b, where b is a column vector. This is more efficient than computing the inverse in one-shot cases. Solve only the upper
    /// 2-by-2 matrix equation.
    Vec2 Solve22(const Vec2& b) const;
    
    /// Get the inverse of this matrix as a 2-by-2. Returns the zero matrix if singular.
    void GetInverse22(Mat33* M) const;
    
    /// Get the symmetric inverse of this matrix as a 3-by-3. Returns the zero matrix if singular.
    void GetSymInverse33(Mat33* M) const;
    
    Vec3 ex, ey, ez;
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
  
  /// This describes the motion of a body/shape for TOI computation. Shapes are defined with respect
  /// to the body origin, which may no coincide with the center of mass. However, to support dynamics
  /// we must interpolate the center of mass position.
  struct Sweep {
    /// Get the interpolated transform at a specific time.
    /// - Parameters:
    ///   - transform: the output transform
    ///   - beta: is a factor in [0,1], where 0 indicates alpha0.
    void GetTransform(Transform* transform, float beta) const;
    
    /// Advance the sweep forward, yielding a new initial state.
    /// - Parameter alpha: the new initial time.
    void Advance(float alpha);
    
    /// Normalize the angles.
    void Normalize();
    
    Vec2 localCenter;  ///< local center of mass position
    Vec2 c0, c;    ///< center world positions
    float a0, a;    ///< world angles
    
    /// Fraction of the current time step in the range [0,1] c0 and a0 are the positions at alpha0.
    float alpha0;
  };

  // -------------------------------------------------------
  // Operators
  // -------------------------------------------------------
  
  /// Perform the cross product on two vectors.
  inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
  }
  
  /// Perform the dot product on two vectors.
  inline float Dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

}
