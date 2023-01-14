//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  class Draw;
  
  /// Distance joint definition. This requires defining an anchor point on both
  /// bodies and the non-zero distance of the distance joint. The definition uses
  /// local anchor points so that the initial configuration can violate the
  /// constraint slightly. This helps when saving and loading a game.
  struct DistanceJointDef : public JointDef {
    DistanceJointDef() {
      type = distanceJoint;
      localAnchorA.Set(0.0f, 0.0f);
      localAnchorB.Set(0.0f, 0.0f);
      length = 1.0f;
      minLength = 0.0f;
      maxLength = FLT_MAX;
      stiffness = 0.0f;
      damping = 0.0f;
    }
    
    /// Initialize the bodies, anchors, and rest length using world space anchors.
    /// The minimum and maximum lengths are set to the rest length.
    void Initialize(Body* bodyA, Body* bodyB,
                    const Vec2& anchorA, const Vec2& anchorB);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The rest length of this joint. Clamped to a stable minimum value.
    float length;
    
    /// Minimum length. Clamped to a stable minimum value.
    float minLength;
    
    /// Maximum length. Must be greater than or equal to the minimum length.
    float maxLength;
    
    /// The linear stiffness in N/m.
    float stiffness;
    
    /// The linear damping in N*s/m.
    float damping;
  };
  
  /// A distance joint constrains two points on two bodies to remain at a fixed
  /// distance from each other. You can view this as a massless, rigid rod.
  class DistanceJoint : public Joint {
  public:
    
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    /// Get the reaction force given the inverse time step.
    /// Unit is N.
    Vec2 GetReactionForce(float inv_dt) const override;
    
    /// Get the reaction torque given the inverse time step.
    /// Unit is N*m. This is always zero for a distance joint.
    float GetReactionTorque(float inv_dt) const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return m_localAnchorA; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return m_localAnchorB; }
    
    /// Get the rest length
    float GetLength() const { return m_length; }
    
    /// Set the rest length
    /// @returns clamped rest length
    float SetLength(float length);
    
    /// Get the minimum length
    float GetMinLength() const { return m_minLength; }
    
    /// Set the minimum length
    /// @returns the clamped minimum length
    float SetMinLength(float minLength);
    
    /// Get the maximum length
    float GetMaxLength() const { return m_maxLength; }
    
    /// Set the maximum length
    /// @returns the clamped maximum length
    float SetMaxLength(float maxLength);
    
    /// Get the current length
    float GetCurrentLength() const;
    
    /// Set/get the linear stiffness in N/m
    void SetStiffness(float stiffness) { m_stiffness = stiffness; }
    float GetStiffness() const { return m_stiffness; }
    
    /// Set/get linear damping in N*s/m
    void SetDamping(float damping) { m_damping = damping; }
    float GetDamping() const { return m_damping; }
    
    void Draw(physics::Draw* draw) const override;
    
  protected:
    
    friend class Joint;
    DistanceJoint(const DistanceJointDef* data);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    float m_stiffness;
    float m_damping;
    float m_bias;
    float m_length;
    float m_minLength;
    float m_maxLength;
    
    // Solver shared
    Vec2 m_localAnchorA;
    Vec2 m_localAnchorB;
    float m_gamma;
    float m_impulse;
    float m_lowerImpulse;
    float m_upperImpulse;
    
    // Solver temp
    int32_t m_indexA;
    int32_t m_indexB;
    Vec2 m_u;
    Vec2 m_rA;
    Vec2 m_rB;
    Vec2 m_localCenterA;
    Vec2 m_localCenterB;
    float m_currentLength;
    float m_invMassA;
    float m_invMassB;
    float m_invIA;
    float m_invIB;
    float m_softMass;
    float m_mass;
  };

  
}
