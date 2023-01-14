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
  struct GearJointDef : public JointDef
  {
    GearJointDef()
    {
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
  class GearJoint : public Joint
  {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// Get the first joint.
    Joint* GetJoint1() { return m_joint1; }
    
    /// Get the second joint.
    Joint* GetJoint2() { return m_joint2; }
    
    /// Set/Get the gear ratio.
    void SetRatio(float ratio);
    float GetRatio() const;
    
  protected:
    
    friend class Joint;
    GearJoint(const GearJointDef* data);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    Joint* m_joint1;
    Joint* m_joint2;
    
    JointType m_typeA;
    JointType m_typeB;
    
    // Body A is connected to body C
    // Body B is connected to body D
    Body* m_bodyC;
    Body* m_bodyD;
    
    // Solver shared
    Vec2 m_localAnchorA;
    Vec2 m_localAnchorB;
    Vec2 m_localAnchorC;
    Vec2 m_localAnchorD;
    
    Vec2 m_localAxisC;
    Vec2 m_localAxisD;
    
    float m_referenceAngleA;
    float m_referenceAngleB;
    
    float m_constant;
    float m_ratio;
    
    float m_impulse;
    
    // Solver temp
    int32_t m_indexA, m_indexB, m_indexC, m_indexD;
    Vec2 m_lcA, m_lcB, m_lcC, m_lcD;
    float m_mA, m_mB, m_mC, m_mD;
    float m_iA, m_iB, m_iC, m_iD;
    Vec2 m_JvAC, m_JvBD;
    float m_JwA, m_JwB, m_JwC, m_JwD;
    float m_mass;
  };

  
}
