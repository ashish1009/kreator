//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Wheel joint definition. This requires defining a line of
  /// motion using an axis and an anchor point. The definition uses local
  /// anchor points and a local axis so that the initial configuration
  /// can violate the constraint slightly. The joint translation is zero
  /// when the local anchor points coincide in world space. Using local
  /// anchors and a local axis helps when saving and loading a game.
  struct WheelJointDef : public JointDef {
    WheelJointDef() {
      type = wheelJoint;
      localAnchorA.SetZero();
      localAnchorB.SetZero();
      localAxisA.Set(1.0f, 0.0f);
      enableLimit = false;
      lowerTranslation = 0.0f;
      upperTranslation = 0.0f;
      enableMotor = false;
      maxMotorTorque = 0.0f;
      motorSpeed = 0.0f;
      stiffness = 0.0f;
      damping = 0.0f;
    }
    
    /// Initialize the bodies, anchors, axis, and reference angle using the world
    /// anchor and world axis.
    void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor, const Vec2& axis);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The local translation axis in bodyA.
    Vec2 localAxisA;
    
    /// Enable/disable the joint limit.
    bool enableLimit;
    
    /// The lower translation limit, usually in meters.
    float lowerTranslation;
    
    /// The upper translation limit, usually in meters.
    float upperTranslation;
    
    /// Enable/disable the joint motor.
    bool enableMotor;
    
    /// The maximum motor torque, usually in N-m.
    float maxMotorTorque;
    
    /// The desired motor speed in radians per second.
    float motorSpeed;
    
    /// Suspension stiffness. Typically in units N/m.
    float stiffness;
    
    /// Suspension damping. Typically in units of N*s/m.
    float damping;
  };
  
  /// A wheel joint. This joint provides two degrees of freedom: translation
  /// along an axis fixed in bodyA and rotation in the plane. In other words, it is a point to
  /// line constraint with a rotational motor and a linear spring/damper. The spring/damper is
  /// initialized upon creation. This joint is designed for vehicle suspensions.
  class WheelJoint : public Joint {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return local_anchor_a_; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return local_anchor_b_; }
    
    /// The local joint axis relative to bodyA.
    const Vec2& GetLocalAxisA() const { return local_x_axis_a_; }
    
    /// Get the current joint translation, usually in meters.
    float GetJointTranslation() const;
    
    /// Get the current joint linear speed, usually in meters per second.
    float GetJointLinearSpeed() const;
    
    /// Get the current joint angle in radians.
    float GetJointAngle() const;
    
    /// Get the current joint angular speed in radians per second.
    float GetJointAngularSpeed() const;
    
    /// Is the joint limit enabled?
    bool IsLimitEnabled() const;
    
    /// Enable/disable the joint translation limit.
    void EnableLimit(bool flag);
    
    /// Get the lower joint translation limit, usually in meters.
    float GetLowerLimit() const;
    
    /// Get the upper joint translation limit, usually in meters.
    float GetUpperLimit() const;
    
    /// Set the joint translation limits, usually in meters.
    void SetLimits(float lower, float upper);
    
    /// Is the joint motor enabled?
    bool IsMotorEnabled() const;
    
    /// Enable/disable the joint motor.
    void EnableMotor(bool flag);
    
    /// Set the motor speed, usually in radians per second.
    void SetMotorSpeed(float speed);
    
    /// Get the motor speed, usually in radians per second.
    float GetMotorSpeed() const;
    
    /// Set/Get the maximum motor force, usually in N-m.
    void SetMaxMotorTorque(float torque);
    float GetMaxMotorTorque() const;
    
    /// Get the current motor torque given the inverse time step, usually in N-m.
    float GetMotorTorque(float inv_dt) const;
    
    /// Access spring stiffness
    void SetStiffness(float stiffness);
    float GetStiffness() const;
    
    /// Access damping
    void SetDamping(float damping);
    float GetDamping() const;
    
    ///
    void Draw(physics::Draw* draw) const override;
    
  protected:
    
    friend class Joint;
    WheelJoint(const WheelJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    Vec2 local_x_axis_a_;
    Vec2 local_y_axis_a_;
    
    float impulse_;
    float motor_impulse_;
    float spring_impusle_;
    
    float lower_impulse_;
    float upper_impulse_;
    float translation_;
    float lower_translation_;
    float upper_translation_;
    
    float max_motor_torque_;
    float motor_speed_;
    
    bool enable_limit_;
    bool enable_motor_;
    
    float stiffness_;
    float damping_;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 local_center_a_;
    Vec2 local_center_b_;
    float inv_mass_a_;
    float inv_mass_b_;
    float inv_i_a_;
    float inv_i_b_;
    
    Vec2 a_x_, a_y_;
    float m_sAx, s_b_x_;
    float s_a_y_, s_b_y_;
    
    float mass_;
    float motor_mass_;
    float axis_mass_;
    float spring_mass_;
    
    float bias_;
    float gamma_;
    
  };
    
}