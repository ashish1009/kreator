//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Prismatic joint definition. This requires defining a line of
  /// motion using an axis and an anchor point. The definition uses local
  /// anchor points and a local axis so that the initial configuration
  /// can violate the constraint slightly. The joint translation is zero
  /// when the local anchor points coincide in world space. Using local
  /// anchors and a local axis helps when saving and loading a game.
  struct PrismaticJointDef : public JointDef {
    PrismaticJointDef()
    {
      type = prismaticJoint;
      localAnchorA.SetZero();
      localAnchorB.SetZero();
      localAxisA.Set(1.0f, 0.0f);
      referenceAngle = 0.0f;
      enableLimit = false;
      lowerTranslation = 0.0f;
      upperTranslation = 0.0f;
      enableMotor = false;
      maxMotorForce = 0.0f;
      motorSpeed = 0.0f;
    }
    
    /// Initialize the bodies, anchors, axis, and reference angle using the world
    /// anchor and unit world axis.
    void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor, const Vec2& axis);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The local translation unit axis in bodyA.
    Vec2 localAxisA;
    
    /// The constrained angle between the bodies: bodyB_angle - bodyA_angle.
    float referenceAngle;
    
    /// Enable/disable the joint limit.
    bool enableLimit;
    
    /// The lower translation limit, usually in meters.
    float lowerTranslation;
    
    /// The upper translation limit, usually in meters.
    float upperTranslation;
    
    /// Enable/disable the joint motor.
    bool enableMotor;
    
    /// The maximum motor torque, usually in N-m.
    float maxMotorForce;
    
    /// The desired motor speed in radians per second.
    float motorSpeed;
  };
  
  /// A prismatic joint. This joint provides one degree of freedom: translation
  /// along an axis fixed in bodyA. Relative rotation is prevented. You can
  /// use a joint limit to restrict the range of motion and a joint motor to
  /// drive the motion or to model joint friction.
  class PrismaticJoint : public Joint {
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
    
    /// Get the reference angle.
    float GetReferenceAngle() const { return reference_angle_; }
    
    /// Get the current joint translation, usually in meters.
    float GetJointTranslation() const;
    
    /// Get the current joint translation speed, usually in meters per second.
    float GetJointSpeed() const;
    
    /// Is the joint limit enabled?
    bool IsLimitEnabled() const;
    
    /// Enable/disable the joint limit.
    void EnableLimit(bool flag);
    
    /// Get the lower joint limit, usually in meters.
    float GetLowerLimit() const;
    
    /// Get the upper joint limit, usually in meters.
    float GetUpperLimit() const;
    
    /// Set the joint limits, usually in meters.
    void SetLimits(float lower, float upper);
    
    /// Is the joint motor enabled?
    bool IsMotorEnabled() const;
    
    /// Enable/disable the joint motor.
    void EnableMotor(bool flag);
    
    /// Set the motor speed, usually in meters per second.
    void SetMotorSpeed(float speed);
    
    /// Get the motor speed, usually in meters per second.
    float GetMotorSpeed() const;
    
    /// Set the maximum motor force, usually in N.
    void SetMaxMotorForce(float force);
    float GetMaxMotorForce() const { return max_motor_force_; }
    
    /// Get the current motor force given the inverse time step, usually in N.
    float GetMotorForce(float inv_dt) const;
    
    ///
    void Draw(physics::Draw* draw) const override;
    
  protected:
    friend class Joint;
    friend class GearJoint;
    PrismaticJoint(const PrismaticJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    Vec2 local_x_axis_a_;
    Vec2 local_y_axis_a_;
    float reference_angle_;
    Vec2 impulse_;
    float motor_impulse_;
    float lower_impulse_;
    float upper_impulse_;
    float lower_translation_;
    float upper_translation_;
    float max_motor_force_;
    float motor_speed_;
    bool enable_limit_;
    bool enable_motor_;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 local_center_a_;
    Vec2 local_center_b_;
    float inv_mass_a_;
    float inv_mass_b_;
    float inv_i_a_;
    float inv_i_b_;
    Vec2 axis_, perp_;
    float s1_, s2_;
    float a1_, a2_;
    Mat22 k_;
    float translation_;
    float axial_mass_;
  };
  
}
