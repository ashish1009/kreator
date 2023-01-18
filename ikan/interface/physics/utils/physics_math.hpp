//
//  physics_math.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
  
  namespace math {
    /// This function is used to ensure that a floating point number is not a NaN or infinity.
    /// - Parameter x: value to be validated
    bool IsValid(float x);
  }
  
  /// This structure stores the vector of 2 floats data
  struct Vec2 {
    Vec2() = default;
    /// This constructor update the x and y value of vector 2D
    /// - Parameters:
    ///   - x_input: x input
    ///   - y_input: y input
    Vec2(float x_input, float y_input);
    
    // ---------------------
    // Fundamentals
    // ---------------------
    /// This function set sthis vector to all zeros.
    void SetZero();
    /// This function sets this vector to some specified coordinates.
    /// - Parameters:
    ///   - x_input: x input
    ///   - y_input: y input
    void Set(float x_input, float y_input);
    
    /// This function convert this vector into a unit vector. Returns the length.
    /// - Important: This function changes the values of x and y !!!
    float Normalize();

    // ---------------------
    // Values
    // ---------------------
    /// This function gets the length of this vector (the norm).
    float Length() const;
    /// Get the length squared. For performance
    /// - Important: Use this instead of Vec2 Length (if possible).
    float LengthSquared() const;
    
    // ---------------------
    // Operators
    // ---------------------
    /// This function negates this vector.
    Vec2 operator -() const;
    /// This function reads from and indexed element.
    /// - Parameter i: index of vector variable
    float operator () (int32_t i) const;
    ///This function write to an indexed element.
    /// - Parameter i: index of vector variable
    float& operator () (int32_t i);
    
    /// This function  add a vector to this vector.
    /// - Parameter v: vector to be added
    void operator += (const Vec2& v);
    /// This function subtracts a vector from this vector.
    /// - Parameter v: vector to be subtracted
    void operator -= (const Vec2& v);
    /// This function multiplies this vector by a scalar.
    /// - Parameter value: value to be multiplied
    void operator *= (float a);
    /// This function divides this vector by a scalar.
    /// - Parameter value: value to be divided
    void operator /= (float a);

    /// This function checks does this vector contain finite coordinates?
    bool IsValid() const;
    /// This function gets the skew vector
    /// - Skew vector is one such that dot(skew_vec, other) == cross(vec, other)
    Vec2 Skew() const;

    // Variabls
    float x, y;
  };
  
  /// This structure stores the vector of 3 floats data
  struct Vec3 {
    Vec3() = default;
    /// This constructor update the x and y value of vector 2D
    /// - Parameters:
    ///   - x_input: x input
    ///   - y_input: y input
    ///   - z_input: z input
    Vec3(float x_input, float y_input, float z_input);
    
    // ---------------------
    // Fundamentals
    // ---------------------
    /// This function set sthis vector to all zeros.
    void SetZero();
    /// This function sets this vector to some specified coordinates.
    /// - Parameters:
    ///   - x_input: x input
    ///   - y_input: y input
    ///   - z_input: z input
    void Set(float x_input, float y_input, float z_input);
    
    /// This function convert this vector into a unit vector. Returns the length.
    /// - Important: This function changes the values of x and y !!!
    float Normalize();
    
    // ---------------------
    // Values
    // ---------------------
    /// This function gets the length of this vector (the norm).
    float Length() const;
    /// Get the length squared. For performance
    /// - Important: Use this instead of Vec2 Length (if possible).
    float LengthSquared() const;
    
    // ---------------------
    // Operators
    // ---------------------
    /// This function negates this vector.
    Vec3 operator -() const;
    /// This function reads from and indexed element.
    /// - Parameter i: index of vector variable
    float operator () (int32_t i) const;
    ///This function write to an indexed element.
    /// - Parameter i: index of vector variable
    float& operator () (int32_t i);
    
    /// This function  add a vector to this vector.
    /// - Parameter v: vector to be added
    void operator += (const Vec3& v);
    /// This function subtracts a vector from this vector.
    /// - Parameter v: vector to be subtracted
    void operator -= (const Vec3& v);
    /// This function multiplies this vector by a scalar.
    /// - Parameter value: value to be multiplied
    void operator *= (float a);
    /// This function divides this vector by a scalar.
    /// - Parameter value: value to be divided
    void operator /= (float a);
    
    /// This function checks does this vector contain finite coordinates?
    bool IsValid() const;
    
    // Variabls
    float x, y, z;
  };
  
}
