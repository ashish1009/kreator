//
//  physics_math.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
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
    glm::vec2 GetXAxis() const { return glm::vec2(c, s); }
    /// Get the u-axis
    glm::vec2 GetYAxis() const { return glm::vec2(-s, c); }
    
    /// Sine and cosine
    float s, c;
  };

  /// A transform contains translation and rotation. It is used to represent the position and orientation of rigid frames.
  struct Transform {
    /// The default constructor does nothing.
    Transform() {}
    /// Initialize using a position vector and a rotation.
    Transform(const glm::vec2& position, const Rot& rotation) : p(position), q(rotation) {}
    
    /// Set this to the identity transform.
    void SetIdentity() {
      p = { 0, 0 };
      q.SetIdentity();
    }
    
    /// Set this based on the position and angle.
    void Set(const glm::vec2& position, float angle) {
      p = position;
      q.Set(angle);
    }
    
    glm::vec2 p;
    Rot q;
  };

}
