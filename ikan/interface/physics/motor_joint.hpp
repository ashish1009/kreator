//
//  motor_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Motor joint definition.
  struct MotorJointDef : public JointDef
  {
    MotorJointDef()
    {
      type = motorJoint;
      linearOffset.SetZero();
      angularOffset = 0.0f;
      maxForce = 1.0f;
      maxTorque = 1.0f;
      correctionFactor = 0.3f;
    }
    
    /// Initialize the bodies and offsets using the current transforms.
    void Initialize(Body* bodyA, Body* bodyB);
    
    /// Position of bodyB minus the position of bodyA, in bodyA's frame, in meters.
    Vec2 linearOffset;
    
    /// The bodyB angle minus bodyA angle in radians.
    float angularOffset;
    
    /// The maximum motor force in N.
    float maxForce;
    
    /// The maximum motor torque in N-m.
    float maxTorque;
    
    /// Position correction factor in the range [0,1].
    float correctionFactor;
  };
  
  /// A motor joint is used to control the relative motion
  /// between two bodies. A typical usage is to control the movement
  /// of a dynamic body with respect to the ground.
  class MotorJoint : public Joint
  {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// Set/get the target linear offset, in frame A, in meters.
    void SetLinearOffset(const Vec2& linearOffset);
    const Vec2& GetLinearOffset() const;
    
    /// Set/get the target angular offset, in radians.
    void SetAngularOffset(float angularOffset);
    float GetAngularOffset() const;
    
    /// Set the maximum friction force in N.
    void SetMaxForce(float force);
    
    /// Get the maximum friction force in N.
    float GetMaxForce() const;
    
    /// Set the maximum friction torque in N*m.
    void SetMaxTorque(float torque);
    
    /// Get the maximum friction torque in N*m.
    float GetMaxTorque() const;
    
    /// Set the position correction factor in the range [0,1].
    void SetCorrectionFactor(float factor);
    
    /// Get the position correction factor in the range [0,1].
    float GetCorrectionFactor() const;
    
    
  protected:
    
    friend class Joint;
    
    MotorJoint(const MotorJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    // Solver shared
    Vec2 linear_offset_;
    float angular_offset_;
    Vec2 linear_impulse_;
    float angular_impulse_;
    float max_force_;
    float max_torque_;
    float correction_factor_;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 ra_;
    Vec2 rb_;
    Vec2 local_center_a_;
    Vec2 local_center_b_;
    Vec2 linear_error_;
    float angular_error_;
    float inv_mass_a_;
    float inv_mass_b_;
    float inv_i_a_;
    float inv_i_b_;
    Mat22 linear_mass_;
    float angular_mass_;
  };

  
}
