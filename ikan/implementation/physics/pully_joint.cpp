//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "pully_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"

namespace physics {
 
  void PulleyJointDef::Initialize(Body* bA, Body* bB,
                                    const Vec2& groundA, const Vec2& groundB,
                                    const Vec2& anchorA, const Vec2& anchorB,
                                    float r) {
    bodyA = bA;
    bodyB = bB;
    groundAnchorA = groundA;
    groundAnchorB = groundB;
    localAnchorA = bodyA->GetLocalPoint(anchorA);
    localAnchorB = bodyB->GetLocalPoint(anchorB);
    Vec2 dA = anchorA - groundA;
    lengthA = dA.Length();
    Vec2 dB = anchorB - groundB;
    lengthB = dB.Length();
    ratio = r;
    IK_ASSERT(ratio > FLT_EPSILON);
  }
  
  PulleyJoint::PulleyJoint(const PulleyJointDef* def)
  : Joint(def) {
    ground_anchor_a_ = def->groundAnchorA;
    ground_anchor_b_ = def->groundAnchorB;
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    
    length_A = def->lengthA;
    length_B = def->lengthB;
    
    IK_ASSERT(def->ratio != 0.0f);
    ra_tio = def->ratio;
    
    constant_ = def->lengthA + ra_tio * def->lengthB;
    
    impulse_ = 0.0f;
  }
  
  void PulleyJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->is_land_index_;
    index_b_ = body_b_->is_land_index_;
    local_center_a_ = body_a_->sweep_.localCenter;
    local_center_b_ = body_b_->sweep_.localCenter;
    inv_mass_a_ = body_a_->inv_mass_;
    inv_mass_b_ = body_b_->inv_mass_;
    inv_i_a_ = body_a_->inv_inertia_;
    inv_i_b_ = body_b_->inv_inertia_;
    
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    Rot qA(aA), qB(aB);
    
    ra_ = Mul(qA, local_anchor_a_ - local_center_a_);
    rb_ = Mul(qB, local_anchor_b_ - local_center_b_);
    
    // Get the pulley axes.
    u_A = cA + ra_ - ground_anchor_a_;
    u_B = cB + rb_ - ground_anchor_b_;
    
    float lengthA = u_A.Length();
    float lengthB = u_B.Length();
    
    if (lengthA > 10.0f * LinearSlop) {
      u_A *= 1.0f / lengthA;
    }
    else {
      u_A.SetZero();
    }
    
    if (lengthB > 10.0f * LinearSlop) {
      u_B *= 1.0f / lengthB;
    }
    else {
      u_B.SetZero();
    }
    
    // Compute effective mass.
    float ruA = Cross(ra_, u_A);
    float ruB = Cross(rb_, u_B);
    
    float mA = inv_mass_a_ + inv_i_a_ * ruA * ruA;
    float mB = inv_mass_b_ + inv_i_b_ * ruB * ruB;
    
    mass_ = mA + ra_tio * ra_tio * mB;
    
    if (mass_ > 0.0f) {
      mass_ = 1.0f / mass_;
    }
    
    if (data.step.warmStarting) {
      // Scale impulses to support variable time steps.
      impulse_ *= data.step.dtRatio;
      
      // Warm starting.
      Vec2 PA = -(impulse_) * u_A;
      Vec2 PB = (-ra_tio * impulse_) * u_B;
      
      vA += inv_mass_a_ * PA;
      wA += inv_i_a_ * Cross(ra_, PA);
      vB += inv_mass_b_ * PB;
      wB += inv_i_b_ * Cross(rb_, PB);
    }
    else {
      impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void PulleyJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    Vec2 vpA = vA + Cross(wA, ra_);
    Vec2 vpB = vB + Cross(wB, rb_);
    
    float Cdot = -Dot(u_A, vpA) - ra_tio * Dot(u_B, vpB);
    float impulse = -mass_ * Cdot;
    impulse_ += impulse;
    
    Vec2 PA = -impulse * u_A;
    Vec2 PB = -ra_tio * impulse * u_B;
    vA += inv_mass_a_ * PA;
    wA += inv_i_a_ * Cross(ra_, PA);
    vB += inv_mass_b_ * PB;
    wB += inv_i_b_ * Cross(rb_, PB);
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool PulleyJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    Rot qA(aA), qB(aB);
    
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    
    // Get the pulley axes.
    Vec2 uA = cA + rA - ground_anchor_a_;
    Vec2 uB = cB + rB - ground_anchor_b_;
    
    float lengthA = uA.Length();
    float lengthB = uB.Length();
    
    if (lengthA > 10.0f * LinearSlop) {
      uA *= 1.0f / lengthA;
    }
    else {
      uA.SetZero();
    }
    
    if (lengthB > 10.0f * LinearSlop) {
      uB *= 1.0f / lengthB;
    }
    else {
      uB.SetZero();
    }
    
    // Compute effective mass.
    float ruA = Cross(rA, uA);
    float ruB = Cross(rB, uB);
    
    float mA = inv_mass_a_ + inv_i_a_ * ruA * ruA;
    float mB = inv_mass_b_ + inv_i_b_ * ruB * ruB;
    
    float mass = mA + ra_tio * ra_tio * mB;
    
    if (mass > 0.0f) {
      mass = 1.0f / mass;
    }
    
    float C = constant_ - lengthA - ra_tio * lengthB;
    float linearError = Abs(C);
    
    float impulse = -mass * C;
    
    Vec2 PA = -impulse * uA;
    Vec2 PB = -ra_tio * impulse * uB;
    
    cA += inv_mass_a_ * PA;
    aA += inv_i_a_ * Cross(rA, PA);
    cB += inv_mass_b_ * PB;
    aB += inv_i_b_ * Cross(rB, PB);
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return linearError < LinearSlop;
  }
  
  Vec2 PulleyJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 PulleyJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 PulleyJoint::GetReactionForce(float inv_dt) const {
    Vec2 P = impulse_ * u_B;
    return inv_dt * P;
  }
  
  float PulleyJoint::GetReactionTorque(float inv_dt) const {
    NOT_USED(inv_dt);
    return 0.0f;
  }
  
  Vec2 PulleyJoint::GetGroundAnchorA() const {
    return ground_anchor_a_;
  }
  
  Vec2 PulleyJoint::GetGroundAnchorB() const {
    return ground_anchor_b_;
  }
  
  float PulleyJoint::GetLengthA() const {
    return length_A;
  }
  
  float PulleyJoint::GetLengthB() const {
    return length_B;
  }
  
  float PulleyJoint::GetRatio() const {
    return ra_tio;
  }
  
  float PulleyJoint::GetCurrentLengthA() const {
    Vec2 p = body_a_->GetWorldPoint(local_anchor_a_);
    Vec2 s = ground_anchor_a_;
    Vec2 d = p - s;
    return d.Length();
  }
  
  float PulleyJoint::GetCurrentLengthB() const {
    Vec2 p = body_b_->GetWorldPoint(local_anchor_b_);
    Vec2 s = ground_anchor_b_;
    Vec2 d = p - s;
    return d.Length();
  }
  
  void PulleyJoint::ShiftOrigin(const Vec2& newOrigin) {
    ground_anchor_a_ -= newOrigin;
    ground_anchor_b_ -= newOrigin;
  }

  
}
