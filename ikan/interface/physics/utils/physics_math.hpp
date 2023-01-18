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
  
  /// This structure stores the rotation information for single axis
  struct Rot {
    Rot() = default;
    
    /// Constructs the rotation from an angle in radians
    /// - Parameter angle: Angle in radians
    explicit Rot(float angle);
    
    /// This function sets the angle value;
    /// - Parameter angle: Angle in radians
    void Set(float angle);
    /// This function sets the angle to the identity rotation
    void SetIdentity();

    /// This function returns the angle in radians
    float GetAngle() const;
    /// This function returns the x-axis using right thumb rule
    Vec2 GetXAxis() const;
    /// This function returns the y-axis using right thumb rule
    Vec2 GetYAxis() const;
    
    /// This function debug print the data
    void Log() const;

    // Member variables
    /// Sine and cosine
    float s, c;
  };
  
  /// This structure stores a transform contains translation and rotation for 2D Space. It is used to represent
  /// the position and orientation of rigid frames.
  struct Transform2D {
    Transform2D() = default;
    
    /// Constructs the Transform using a position vector and a rotation.
    /// - Parameters:
    ///   - position: 2D Position Vector
    ///   - rotation: rotation at Z Axis in radians
    Transform2D(const Vec2& position, const Rot& rotation);
    
    /// Set this to the identity transform.
    void SetIdentity();
    /// Set this based on the position and angle.
    /// - Parameters:
    ///   - position: 2D Position Vector
    ///   - angle: rotation at Z Axis in radians
    void Set(const Vec2& position, float angle);
    
    /// This function debug print the data
    void Log() const;
    
    // Variables
    Vec2 p;
    Rot q;
  };
  
  /// This strucrture describes the motion of a body/shape for time of impact computation. Shapes are
  /// defined with respect to the body origin, which may no coincide with the center of mass. However,
  /// to support dynamics we must interpolate the center of mass position.
  /// - Important: https://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
  struct Sweep {
    /// This functuion interpolates transform at a specific time.
    /// - Parameters:
    ///   - transform: The output transform 2D  data
    ///   - beta: is a factor in [0,1], where 0 indicates alpha0.
    void GetTransform(Transform2D* transform, float beta) const;
    /// This functuion advances the sweep forward, yielding a new initial state.
    /// - Parameter alpha: the new initial time.
    void Advance(float alpha);
    
    /// Normalize the angles.
    void Normalize();
    
    // Variables
    Vec2 local_center;  /// local center of mass position
    Vec2 c0, c;         /// center world positions
    float a0, a;        /// world angles
    float alpha0;       /// Fraction of the current time step in the range [0,1] c0 and a0 are the positions at alpha0.
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
    
    /// This function rotates a vector by rotation defined in q
    /// - Parameters:
    ///   - q: Rotation Data
    ///   - v: Vector of 2 float to be rotated
    Vec2 Mul(const Rot& q, const Vec2& v);
    
    /// This function adds two vectors (2 Floats) component-wise.
    Vec2 operator + (const Vec2& a, const Vec2& b);
    /// This function subtracts two vectors (2 Floats) component-wise.
    Vec2 operator - (const Vec2& a, const Vec2& b);
    /// This function performs multiply the Vector 2 with a scalar value
    /// - Parameters:
    ///   - s: Scalar value
    ///   - a: Vector 2
    Vec2 operator* (float s, const Vec2& a);

  }

}
