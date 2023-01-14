//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Friction joint definition.
  struct FrictionJointDef : public JointDef
  {
    FrictionJointDef()
    {
      type = frictionJoint;
      localAnchorA.SetZero();
      localAnchorB.SetZero();
      maxForce = 0.0f;
      maxTorque = 0.0f;
    }
    
    /// Initialize the bodies, anchors, axis, and reference angle using the world
    /// anchor and world axis.
    void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The maximum friction force in N.
    float maxForce;
    
    /// The maximum friction torque in N-m.
    float maxTorque;
  };
  
  /// Friction joint. This is used for top-down friction.
  /// It provides 2D translational friction and angular friction.
  class FrictionJoint : public Joint
  {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return local_anchor_a_; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return local_anchor_b_; }
    
    /// Set the maximum friction force in N.
    void SetMaxForce(float force);
    
    /// Get the maximum friction force in N.
    float GetMaxForce() const;
    
    /// Set the maximum friction torque in N*m.
    void SetMaxTorque(float torque);
    
    /// Get the maximum friction torque in N*m.
    float GetMaxTorque() const;
    
    
  protected:
    
    friend class Joint;
    
    FrictionJoint(const FrictionJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    
    // Solver shared
    Vec2 linear_impulse_;
    float angular_impulse_;
    float max_force_;
    float max_torque_;
    
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
    Mat22 linear_mass_;
    float angular_mass_;
  };

  
}
