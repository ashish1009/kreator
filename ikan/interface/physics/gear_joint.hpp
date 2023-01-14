//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Gear joint definition. This definition requires two existing
  /// revolute or prismatic joints (any combination will work).
  /// @warning bodyB on the input joints must both be dynamic
  struct GearJointDef : public JointDef {
    GearJointDef() {
      type = gearJoint;
      joint1 = nullptr;
      joint2 = nullptr;
      ratio = 1.0f;
    }
    
    /// The first revolute/prismatic joint attached to the gear joint.
    Joint* joint1;
    
    /// The second revolute/prismatic joint attached to the gear joint.
    Joint* joint2;
    
    /// The gear ratio.
    /// @see GearJoint for explanation.
    float ratio;
  };
  
  /// A gear joint is used to connect two joints together. Either joint
  /// can be a revolute or prismatic joint. You specify a gear ratio
  /// to bind the motions together:
  /// coordinate1 + ratio * coordinate2 = constant
  /// The ratio can be negative or positive. If one joint is a revolute joint
  /// and the other joint is a prismatic joint, then the ratio will have units
  /// of length or units of 1/length.
  /// @warning You have to manually destroy the gear joint if joint1 or joint2
  /// is destroyed.
  class GearJoint : public Joint {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// Get the first joint.
    Joint* GetJoint1() { return joint_1_; }
    
    /// Get the second joint.
    Joint* GetJoint2() { return joint_2_; }
    
    /// Set/Get the gear ratio.
    void SetRatio(float ratio);
    float GetRatio() const;
    
  protected:
    
    friend class Joint;
    GearJoint(const GearJointDef* data);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Joint* joint_1_;
    Joint* joint_2_;
    
    JointType type_a_;
    JointType type_b_;
    
    // Body A is connected to body C
    // Body B is connected to body D
    Body* body_c_;
    Body* body_d_;
    
    // Solver shared
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    Vec2 local_anchor_c_;
    Vec2 local_anchor_d_;
    
    Vec2 local_axis_c_;
    Vec2 local_axis_d_;
    
    float reference_angle_A;
    float reference_angle_B;
    
    float constant_;
    float ra_tio;
    
    float impulse_;
    
    // Solver temp
    int32_t index_a_, index_b_, m_indexC, m_indexD;
    Vec2 lc_a_, lc_b_, lc_c_, lc_d_;
    float m_a_, m_b_, m_c_, m_d_;
    float m_iA, i_b_, i_c_, i_d_;
    Vec2 jv_ac_, jv_bd_;
    float jw_a_, jw_b_, jw_c_, jw_d_;
    float mass_;
  };

  
}
