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
  struct WheelJointDef : public JointDef
  {
    WheelJointDef()
    {
      type = e_wheelJoint;
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
  class WheelJoint : public Joint
  {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return m_localAnchorA; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return m_localAnchorB; }
    
    /// The local joint axis relative to bodyA.
    const Vec2& GetLocalAxisA() const { return m_localXAxisA; }
    
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
    
    Vec2 m_localAnchorA;
    Vec2 m_localAnchorB;
    Vec2 m_localXAxisA;
    Vec2 m_localYAxisA;
    
    float m_impulse;
    float m_motorImpulse;
    float m_springImpulse;
    
    float m_lowerImpulse;
    float m_upperImpulse;
    float m_translation;
    float m_lowerTranslation;
    float m_upperTranslation;
    
    float m_maxMotorTorque;
    float m_motorSpeed;
    
    bool m_enableLimit;
    bool m_enableMotor;
    
    float m_stiffness;
    float m_damping;
    
    // Solver temp
    int32_t m_indexA;
    int32_t m_indexB;
    Vec2 m_localCenterA;
    Vec2 m_localCenterB;
    float m_invMassA;
    float m_invMassB;
    float m_invIA;
    float m_invIB;
    
    Vec2 m_ax, m_ay;
    float m_sAx, m_sBx;
    float m_sAy, m_sBy;
    
    float m_mass;
    float m_motorMass;
    float m_axialMass;
    float m_springMass;
    
    float m_bias;
    float m_gamma;
    
  };
  
  inline float WheelJoint::GetMotorSpeed() const
  {
    return m_motorSpeed;
  }
  
  inline float WheelJoint::GetMaxMotorTorque() const
  {
    return m_maxMotorTorque;
  }

  
}
