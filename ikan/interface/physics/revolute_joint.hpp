//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Revolute joint definition. This requires defining an anchor point where the
  /// bodies are joined. The definition uses local anchor points so that the
  /// initial configuration can violate the constraint slightly. You also need to
  /// specify the initial relative angle for joint limits. This helps when saving
  /// and loading a game.
  /// The local anchor points are measured from the body's origin
  /// rather than the center of mass because:
  /// 1. you might not know where the center of mass will be.
  /// 2. if you add/remove shapes from a body and recompute the mass,
  ///    the joints will be broken.
  struct RevoluteJointDef : public JointDef {
    RevoluteJointDef() {
      type = revoluteJoint;
      localAnchorA.Set(0.0f, 0.0f);
      localAnchorB.Set(0.0f, 0.0f);
      referenceAngle = 0.0f;
      lowerAngle = 0.0f;
      upperAngle = 0.0f;
      maxMotorTorque = 0.0f;
      motorSpeed = 0.0f;
      enableLimit = false;
      enableMotor = false;
    }
    
    /// Initialize the bodies, anchors, and reference angle using a world
    /// anchor point.
    void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The bodyB angle minus bodyA angle in the reference state (radians).
    float referenceAngle;
    
    /// A flag to enable joint limits.
    bool enableLimit;
    
    /// The lower angle for the joint limit (radians).
    float lowerAngle;
    
    /// The upper angle for the joint limit (radians).
    float upperAngle;
    
    /// A flag to enable the joint motor.
    bool enableMotor;
    
    /// The desired motor speed. Usually in radians per second.
    float motorSpeed;
    
    /// The maximum motor torque used to achieve the desired motor speed.
    /// Usually in N-m.
    float maxMotorTorque;
  };
  
  /// A revolute joint constrains two bodies to share a common point while they
  /// are free to rotate about the point. The relative rotation about the shared
  /// point is the joint angle. You can limit the relative rotation with
  /// a joint limit that specifies a lower and upper angle. You can use a motor
  /// to drive the relative rotation about the shared point. A maximum motor torque
  /// is provided so that infinite forces are not generated.
  class RevoluteJoint : public Joint {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return local_anchor_a_; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return local_anchor_b_; }
    
    /// Get the reference angle.
    float GetReferenceAngle() const { return reference_angle_; }
    
    /// Get the current joint angle in radians.
    float GetJointAngle() const;
    
    /// Get the current joint angle speed in radians per second.
    float GetJointSpeed() const;
    
    /// Is the joint limit enabled?
    bool IsLimitEnabled() const;
    
    /// Enable/disable the joint limit.
    void EnableLimit(bool flag);
    
    /// Get the lower joint limit in radians.
    float GetLowerLimit() const;
    
    /// Get the upper joint limit in radians.
    float GetUpperLimit() const;
    
    /// Set the joint limits in radians.
    void SetLimits(float lower, float upper);
    
    /// Is the joint motor enabled?
    bool IsMotorEnabled() const;
    
    /// Enable/disable the joint motor.
    void EnableMotor(bool flag);
    
    /// Set the motor speed in radians per second.
    void SetMotorSpeed(float speed);
    
    /// Get the motor speed in radians per second.
    float GetMotorSpeed() const;
    
    /// Set the maximum motor torque, usually in N-m.
    void SetMaxMotorTorque(float torque);
    float GetMaxMotorTorque() const { return max_motor_torque_; }
    
    /// Get the reaction force given the inverse time step.
    /// Unit is N.
    Vec2 GetReactionForce(float inv_dt) const override;
    
    /// Get the reaction torque due to the joint limit given the inverse time step.
    /// Unit is N*m.
    float GetReactionTorque(float inv_dt) const override;
    
    /// Get the current motor torque given the inverse time step.
    /// Unit is N*m.
    float GetMotorTorque(float inv_dt) const;
    
    ///
    void Draw(physics::Draw* draw) const override;
    
  protected:
    
    friend class Joint;
    friend class GearJoint;
    
    RevoluteJoint(const RevoluteJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    // Solver shared
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    Vec2 impulse_;
    float motor_impulse_;
    float lower_impulse_;
    float upper_impulse_;
    bool enable_motor_;
    float max_motor_torque_;
    float motor_speed_;
    bool enable_limit_;
    float reference_angle_;
    float m_lowerAngle;
    float u_pperAngle;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 ra_;
    Vec2 rb_;
    Vec2 local_center_a_;
    Vec2 local_center_b_;
    float inv_mass_a_;
    float inv_mass_b_;
    float inv_i_a_;
    float inv_i_b_;
    Mat22 k_;
    float angle_;
    float axial_mass_;
  };
  
}
