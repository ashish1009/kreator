//
//  physics_math.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
    
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
    
    /// This function debug print the data
    void Log() const;

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
    
    /// This function debug print the data
    void Log() const;

    // Variabls
    float x, y, z;
  };
  
  /// This structure stores a 2-by-2 matrix. Stored in column-major order.
  struct Mat22 {
    Mat22() = default;
    
    // --------------------
    // Constructors
    // --------------------
    /// Construct this matrix using columns.
    /// - Parameters:
    ///   - c1: Column 1
    ///   - c2: Column 2
    Mat22(const Vec2& c1, const Vec2& c2);
    /// Construct this matrix using scalars.
    /// - Parameters:
    ///   - a11: value [0][0]
    ///   - a12: value [0][1]
    ///   - a21: value [1][0]
    ///   - a22: value [1][1]
    Mat22(float a11, float a12, float a21, float a22);
    
    // -----------------
    // Fundamentals
    // -----------------
    /// This function initializes this matrix using columns.
    /// - Parameters:
    ///   - c1: Column 1
    ///   - c2: Column 2
    void Set(const Vec2& c1, const Vec2& c2);
    /// This function sets this to the identity matrix.
    void SetIdentity();
    /// This function sets this matrix to all zeros.
    void SetZero();
    /// This function calculate the inverse matrix and return new copy
    Mat22 GetInverse() const;
    
    /// This function multiply the Mat22 with Vec2
    ///  -  this->Mat22 * x = resunt Vec2,
    ///    - where b is a column vector.
    /// - Parameter x: vector to be multiplied with this matrix
    /// - Important: This is more efficient than computing the inverse in one-shot cases.
    Vec2 Solve(const Vec2& x) const;
    
    /// This function debug print the data
    void Log() const;

    // Variables
    Vec2 ex, ey;
  };
 
  /// This structure stores a 2-by-2 matrix. Stored in column-major order.
  struct Mat33 {
    Mat33() = default;
    
    // --------------------
    // Constructors
    // --------------------
    /// Construct this matrix using columns.
    /// - Parameters:
    ///   - c1: Column 1
    ///   - c2: Column 2
    ///   - c3: Column 3
    Mat33(const Vec3& c1, const Vec3& c2, const Vec3& c3);
    /// This function initializes this matrix using columns.
    /// - Parameters:
    ///   - c1: Column 1
    ///   - c2: Column 2
    ///   - c3: Column 3
    void Set(const Vec3& c1, const Vec3& c2, const Vec3& c3);
    /// This function sets this to the identity matrix.
    void SetIdentity();
    /// This function sets this matrix to all zeros.
    void SetZero();

    /// This function calculates the inverse of this matrix as a 2-by-2.
    /// - Parameter M: Output Transform Matrix (last column and last row set as 0.0f)
    /// - Important: Returns the zero matrix if singular.
    void GetInverse22(Mat33* M) const;
    
    /// This function calculates the inverse of this matrix as a 3-by-3.
    /// - Parameter M: Output Transform Matrix.
    /// - Important: Returns the zero matrix if singular.
    void GetSymInverse33(Mat33* M) const;

    /// This function multiplies the Mat33 with Vec3
    /// - this Mat33 * b = return Vec3,
    /// - Parameter b: Vector to be multiplied
    /// - Important: This is more efficient than computing the inverse in one-shot cases.
    Vec3 Solve33(const Vec3& b) const;
    
    /// This function multiplies the Mat33 with Vec2
    /// - this Mat33 * b = return Vec2
    /// - Parameter b: Vector to be multiplied
    /// - Important: This is more efficient than computing the inverse in one-shot cases.
    ///               Solve only the upper 2-by-2 matrix equation.
    Vec2 Solve22(const Vec2& b) const;

    /// This function debug print the data
    void Log() const;

    // Variables
    Vec3 ex, ey, ez;
  };
  
  namespace math {
    
    /// This function is used to ensure that a floating point number is not a NaN or infinity.
    /// - Parameter x: value to be validated
    bool IsValid(float x);
    
    /// This function performs the dot product of 2 Vectors of 3 floats
    /// - Parameters:
    ///   - a: Vector 3 float a
    ///   - b: Vector 3 float b
    float Dot(const Vec3& a, const Vec3& b);
    /// This function performs the cross product of 2 Vectors of 3 floats
    /// - Parameters:
    ///   - a: Vector 3 float a
    ///   - b: Vector 3 float b
    Vec3 Cross(const Vec3& a, const Vec3& b);
    
  }

}
