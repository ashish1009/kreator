//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  const float _minPulleyLength = 2.0f;
  
  /// Pulley joint definition. This requires two ground anchors,
  /// two dynamic body anchor points, and a pulley ratio.
  struct PulleyJointDef : public JointDef {
    PulleyJointDef() {
      type = pulleyJoint;
      groundAnchorA.Set(-1.0f, 1.0f);
      groundAnchorB.Set(1.0f, 1.0f);
      localAnchorA.Set(-1.0f, 0.0f);
      localAnchorB.Set(1.0f, 0.0f);
      lengthA = 0.0f;
      lengthB = 0.0f;
      ratio = 1.0f;
      collideConnected = true;
    }
    
    /// Initialize the bodies, anchors, lengths, max lengths, and ratio using the world anchors.
    void Initialize(Body* bodyA, Body* bodyB,
                    const Vec2& groundAnchorA, const Vec2& groundAnchorB,
                    const Vec2& anchorA, const Vec2& anchorB,
                    float ratio);
    
    /// The first ground anchor in world coordinates. This point never moves.
    Vec2 groundAnchorA;
    
    /// The second ground anchor in world coordinates. This point never moves.
    Vec2 groundAnchorB;
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The a reference length for the segment attached to bodyA.
    float lengthA;
    
    /// The a reference length for the segment attached to bodyB.
    float lengthB;
    
    /// The pulley ratio, used to simulate a block-and-tackle.
    float ratio;
  };
  
  /// The pulley joint is connected to two bodies and two fixed ground points.
  /// The pulley supports a ratio such that:
  /// length1 + ratio * length2 <= constant
  /// Yes, the force transmitted is scaled by the ratio.
  /// Warning: the pulley joint can get a bit squirrelly by itself. They often
  /// work better when combined with prismatic joints. You should also cover the
  /// the anchor points with static shapes to prevent one side from going to
  /// zero length.
  class PulleyJoint : public Joint {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// Get the first ground anchor.
    Vec2 GetGroundAnchorA() const;
    
    /// Get the second ground anchor.
    Vec2 GetGroundAnchorB() const;
    
    /// Get the current length of the segment attached to bodyA.
    float GetLengthA() const;
    
    /// Get the current length of the segment attached to bodyB.
    float GetLengthB() const;
    
    /// Get the pulley ratio.
    float GetRatio() const;
    
    /// Get the current length of the segment attached to bodyA.
    float GetCurrentLengthA() const;
    
    /// Get the current length of the segment attached to bodyB.
    float GetCurrentLengthB() const;
    
    /// Implement Joint::ShiftOrigin
    void ShiftOrigin(const Vec2& newOrigin) override;
    
  protected:
    
    friend class Joint;
    PulleyJoint(const PulleyJointDef* data);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Vec2 ground_anchor_a_;
    Vec2 ground_anchor_b_;
    float length_A;
    float length_B;
    
    // Solver shared
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    float constant_;
    float ra_tio;
    float impulse_;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 u_A;
    Vec2 u_B;
    Vec2 ra_;
    Vec2 rb_;
    Vec2 local_center_a_;
    Vec2 local_center_b_;
    float inv_mass_a_;
    float inv_mass_b_;
    float inv_i_a_;
    float inv_i_b_;
    float mass_;
  };

  
}
