//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Mouse joint definition. This requires a world target point,
  /// tuning parameters, and the time step.
  struct MouseJointDef : public JointDef {
    MouseJointDef() {
      type = mouseJoint;
      target.Set(0.0f, 0.0f);
      maxForce = 0.0f;
      stiffness = 0.0f;
      damping = 0.0f;
    }
    
    /// The initial world target point. This is assumed
    /// to coincide with the body anchor initially.
    Vec2 target;
    
    /// The maximum constraint force that can be exerted
    /// to move the candidate body. Usually you will express
    /// as some multiple of the weight (multiplier * mass * gravity).
    float maxForce;
    
    /// The linear stiffness in N/m
    float stiffness;
    
    /// The linear damping in N*s/m
    float damping;
  };
  
  /// A mouse joint is used to make a point on a body track a
  /// specified world point. This a soft constraint with a maximum
  /// force. This allows the constraint to stretch and without
  /// applying huge forces.
  /// NOTE: this joint is not documented in the manual because it was
  /// developed to be used in the testbed. If you want to learn how to
  /// use the mouse joint, look at the testbed.
  class MouseJoint : public Joint {
  public:
    
    /// Implements Joint.
    Vec2 GetAnchorA() const override;
    
    /// Implements Joint.
    Vec2 GetAnchorB() const override;
    
    /// Implements Joint.
    Vec2 GetReactionForce(float inv_dt) const override;
    
    /// Implements Joint.
    float GetReactionTorque(float inv_dt) const override;
    
    /// Use this to update the target point.
    void SetTarget(const Vec2& target);
    const Vec2& GetTarget() const;
    
    /// Set/get the maximum force in Newtons.
    void SetMaxForce(float force);
    float GetMaxForce() const;
    
    /// Set/get the linear stiffness in N/m
    void SetStiffness(float stiffness) { stiffness_ = stiffness; }
    float GetStiffness() const { return stiffness_; }
    
    /// Set/get linear damping in N*s/m
    void SetDamping(float damping) { damping_ = damping; }
    float GetDamping() const { return damping_; }
    
    /// Implement Joint::ShiftOrigin
    void ShiftOrigin(const Vec2& newOrigin) override;
    
  protected:
    friend class Joint;
    
    MouseJoint(const MouseJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Vec2 local_anchor_b_;
    Vec2 target_a_;
    float stiffness_;
    float damping_;
    float beta_;
    
    // Solver shared
    Vec2 impulse_;
    float max_force_;
    float gamma_;
    
    // Solver temp
    int32_t index_a_;
    int32_t index_b_;
    Vec2 rb_;
    Vec2 local_center_b_;
    float inv_mass_b_;
    float inv_i_b_;
    Mat22 mass_;
    Vec2 c_;
  };
    
}
