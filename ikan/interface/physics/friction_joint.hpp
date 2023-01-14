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
    const Vec2& GetLocalAnchorA() const { return m_localAnchorA; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return m_localAnchorB; }
    
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
    
    Vec2 m_localAnchorA;
    Vec2 m_localAnchorB;
    
    // Solver shared
    Vec2 m_linearImpulse;
    float m_angularImpulse;
    float m_maxForce;
    float m_maxTorque;
    
    // Solver temp
    int32_t m_indexA;
    int32_t m_indexB;
    Vec2 m_rA;
    Vec2 m_rB;
    Vec2 m_localCenterA;
    Vec2 m_localCenterB;
    float m_invMassA;
    float m_invMassB;
    float m_invIA;
    float m_invIB;
    Mat22 m_linearMass;
    float m_angularMass;
  };

  
}
