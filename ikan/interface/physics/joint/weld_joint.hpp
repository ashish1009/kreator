//
//  distance_joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "joint.hpp"

namespace physics {
  
  /// Weld joint definition. You need to specify local anchor points
  /// where they are attached and the relative body angle. The position
  /// of the anchor points is important for computing the reaction torque.
  struct WeldJointDef : public JointDef {
    WeldJointDef() {
      type = weldJoint;
      localAnchorA.Set(0.0f, 0.0f);
      localAnchorB.Set(0.0f, 0.0f);
      referenceAngle = 0.0f;
      stiffness = 0.0f;
      damping = 0.0f;
    }
    
    /// Initialize the bodies, anchors, reference angle, stiffness, and damping.
    /// @param bodyA the first body connected by this joint
    /// @param bodyB the second body connected by this joint
    /// @param anchor the point of connection in world coordinates
    void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor);
    
    /// The local anchor point relative to bodyA's origin.
    Vec2 localAnchorA;
    
    /// The local anchor point relative to bodyB's origin.
    Vec2 localAnchorB;
    
    /// The bodyB angle minus bodyA angle in the reference state (radians).
    float referenceAngle;
    
    /// The rotational stiffness in N*m
    /// Disable softness with a value of 0
    float stiffness;
    
    /// The rotational damping in N*m*s
    float damping;
  };
  
  /// A weld joint essentially glues two bodies together. A weld joint may
  /// distort somewhat because the island constraint solver is approximate.
  class WeldJoint : public Joint {
  public:
    Vec2 GetAnchorA() const override;
    Vec2 GetAnchorB() const override;
    
    Vec2 GetReactionForce(float inv_dt) const override;
    float GetReactionTorque(float inv_dt) const override;
    
    /// The local anchor point relative to bodyA's origin.
    const Vec2& GetLocalAnchorA() const { return local_anchor_a_; }
    
    /// The local anchor point relative to bodyB's origin.
    const Vec2& GetLocalAnchorB() const  { return local_anchor_b_; }
    
    /// Get the reference angle.
    float GetReferenceAngle() const { return reference_angle_; }
    
    /// Set/get stiffness in N*m
    void SetStiffness(float hz) { stiffness_ = hz; }
    float GetStiffness() const { return stiffness_; }
    
    /// Set/get damping in N*m*s
    void SetDamping(float damping) { damping_ = damping; }
    float GetDamping() const { return damping_; }
        
  protected:
    
    friend class Joint;
    
    WeldJoint(const WeldJointDef* def);
    
    void InitVelocityConstraints(const SolverData& data) override;
    void SolveVelocityConstraints(const SolverData& data) override;
    bool SolvePositionConstraints(const SolverData& data) override;
    
    float stiffness_;
    float damping_;
    float bias_;
    
    // Solver shared
    Vec2 local_anchor_a_;
    Vec2 local_anchor_b_;
    float reference_angle_;
    float gamma_;
    Vec3 impulse_;
    
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
    Mat33 mass_;
  };

  
}
