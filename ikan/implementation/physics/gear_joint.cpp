//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
  
  GearJoint::GearJoint(const GearJointDef* def)
  : Joint(def) {
    joint_1_ = def->joint1;
    joint_2_ = def->joint2;
    
    type_a_ = joint_1_->GetType();
    type_b_ = joint_2_->GetType();
    
    IK_ASSERT(type_a_ == revoluteJoint || type_a_ == prismaticJoint);
    IK_ASSERT(type_b_ == revoluteJoint || type_b_ == prismaticJoint);
    
    float coordinateA, coordinateB;
    
    // TODO_ERIN there might be some problem with the joint edges in Joint.
    
    body_c_ = joint_1_->GetBodyA();
    body_a_ = joint_1_->GetBodyB();
    
    // Body B on joint1 must be dynamic
    IK_ASSERT(body_a_->type_ == BodyType::DynamicBody);
    
    // Get geometry of joint1
    Transform xfA = body_a_->xf_;
    float aA = body_a_->sweep_.a;
    Transform xfC = body_c_->xf_;
    float aC = body_c_->sweep_.a;
    
    if (type_a_ == revoluteJoint) {
      RevoluteJoint* revolute = (RevoluteJoint*)def->joint1;
      local_anchor_c_ = revolute->local_anchor_a_;
      local_anchor_a_ = revolute->local_anchor_b_;
      reference_angle_A = revolute->reference_angle_;
      local_axis_c_.SetZero();
      
      coordinateA = aA - aC - reference_angle_A;
    }
    else {
      PrismaticJoint* prismatic = (PrismaticJoint*)def->joint1;
      local_anchor_c_ = prismatic->local_anchor_a_;
      local_anchor_a_ = prismatic->local_anchor_b_;
      reference_angle_A = prismatic->reference_angle_;
      local_axis_c_ = prismatic->local_x_axis_a_;
      
      Vec2 pC = local_anchor_c_;
      Vec2 pA = MulT(xfC.q, Mul(xfA.q, local_anchor_a_) + (xfA.p - xfC.p));
      coordinateA = Dot(pA - pC, local_axis_c_);
    }
    
    body_d_ = joint_2_->GetBodyA();
    body_b_ = joint_2_->GetBodyB();
    
    // Body B on joint2 must be dynamic
    IK_ASSERT(body_b_->type_ == BodyType::DynamicBody);
    
    // Get geometry of joint2
    Transform xfB = body_b_->xf_;
    float aB = body_b_->sweep_.a;
    Transform xfD = body_d_->xf_;
    float aD = body_d_->sweep_.a;
    
    if (type_b_ == revoluteJoint) {
      RevoluteJoint* revolute = (RevoluteJoint*)def->joint2;
      local_anchor_d_ = revolute->local_anchor_a_;
      local_anchor_b_ = revolute->local_anchor_b_;
      reference_angle_B = revolute->reference_angle_;
      local_axis_d_.SetZero();
      
      coordinateB = aB - aD - reference_angle_B;
    }
    else {
      PrismaticJoint* prismatic = (PrismaticJoint*)def->joint2;
      local_anchor_d_ = prismatic->local_anchor_a_;
      local_anchor_b_ = prismatic->local_anchor_b_;
      reference_angle_B = prismatic->reference_angle_;
      local_axis_d_ = prismatic->local_x_axis_a_;
      
      Vec2 pD = local_anchor_d_;
      Vec2 pB = MulT(xfD.q, Mul(xfB.q, local_anchor_b_) + (xfB.p - xfD.p));
      coordinateB = Dot(pB - pD, local_axis_d_);
    }
    
    ra_tio = def->ratio;
    
    constant_ = coordinateA + ra_tio * coordinateB;
    
    impulse_ = 0.0f;
  }
  
  void GearJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->is_land_index_;
    index_b_ = body_b_->is_land_index_;
    m_indexC = body_c_->is_land_index_;
    m_indexD = body_d_->is_land_index_;
    lc_a_ = body_a_->sweep_.localCenter;
    lc_b_ = body_b_->sweep_.localCenter;
    lc_c_ = body_c_->sweep_.localCenter;
    lc_d_ = body_d_->sweep_.localCenter;
    m_a_ = body_a_->inv_mass_;
    m_b_ = body_b_->inv_mass_;
    m_c_ = body_c_->inv_mass_;
    m_d_ = body_d_->inv_mass_;
    m_iA = body_a_->inv_inertia_;
    i_b_ = body_b_->inv_inertia_;
    i_c_ = body_c_->inv_inertia_;
    i_d_ = body_d_->inv_inertia_;
    
    float aA = data.positions[index_a_].a;
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    
    float aB = data.positions[index_b_].a;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float aC = data.positions[m_indexC].a;
    Vec2 vC = data.velocities[m_indexC].v;
    float wC = data.velocities[m_indexC].w;
    
    float aD = data.positions[m_indexD].a;
    Vec2 vD = data.velocities[m_indexD].v;
    float wD = data.velocities[m_indexD].w;
    
    Rot qA(aA), qB(aB), qC(aC), qD(aD);
    
    mass_ = 0.0f;
    
    if (type_a_ == revoluteJoint) {
      jv_ac_.SetZero();
      jw_a_ = 1.0f;
      jw_c_ = 1.0f;
      mass_ += m_iA + i_c_;
    }
    else {
      Vec2 u = Mul(qC, local_axis_c_);
      Vec2 rC = Mul(qC, local_anchor_c_ - lc_c_);
      Vec2 rA = Mul(qA, local_anchor_a_ - lc_a_);
      jv_ac_ = u;
      jw_c_ = Cross(rC, u);
      jw_a_ = Cross(rA, u);
      mass_ += m_c_ + m_a_ + i_c_ * jw_c_ * jw_c_ + m_iA * jw_a_ * jw_a_;
    }
    
    if (type_b_ == revoluteJoint) {
      jv_bd_.SetZero();
      jw_b_ = ra_tio;
      jw_d_ = ra_tio;
      mass_ += ra_tio * ra_tio * (i_b_ + i_d_);
    }
    else {
      Vec2 u = Mul(qD, local_axis_d_);
      Vec2 rD = Mul(qD, local_anchor_d_ - lc_d_);
      Vec2 rB = Mul(qB, local_anchor_b_ - lc_b_);
      jv_bd_ = ra_tio * u;
      jw_d_ = ra_tio * Cross(rD, u);
      jw_b_ = ra_tio * Cross(rB, u);
      mass_ += ra_tio * ra_tio * (m_d_ + m_b_) + i_d_ * jw_d_ * jw_d_ + i_b_ * jw_b_ * jw_b_;
    }
    
    // Compute effective mass.
    mass_ = mass_ > 0.0f ? 1.0f / mass_ : 0.0f;
    
    if (data.step.warm_starting) {
      vA += (m_a_ * impulse_) * jv_ac_;
      wA += m_iA * impulse_ * jw_a_;
      vB += (m_b_ * impulse_) * jv_bd_;
      wB += i_b_ * impulse_ * jw_b_;
      vC -= (m_c_ * impulse_) * jv_ac_;
      wC -= i_c_ * impulse_ * jw_c_;
      vD -= (m_d_ * impulse_) * jv_bd_;
      wD -= i_d_ * impulse_ * jw_d_;
    }
    else {
      impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
    data.velocities[m_indexC].v = vC;
    data.velocities[m_indexC].w = wC;
    data.velocities[m_indexD].v = vD;
    data.velocities[m_indexD].w = wD;
  }
  
  void GearJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    Vec2 vC = data.velocities[m_indexC].v;
    float wC = data.velocities[m_indexC].w;
    Vec2 vD = data.velocities[m_indexD].v;
    float wD = data.velocities[m_indexD].w;
    
    float Cdot = Dot(jv_ac_, vA - vC) + Dot(jv_bd_, vB - vD);
    Cdot += (jw_a_ * wA - jw_c_ * wC) + (jw_b_ * wB - jw_d_ * wD);
    
    float impulse = -mass_ * Cdot;
    impulse_ += impulse;
    
    vA += (m_a_ * impulse) * jv_ac_;
    wA += m_iA * impulse * jw_a_;
    vB += (m_b_ * impulse) * jv_bd_;
    wB += i_b_ * impulse * jw_b_;
    vC -= (m_c_ * impulse) * jv_ac_;
    wC -= i_c_ * impulse * jw_c_;
    vD -= (m_d_ * impulse) * jv_bd_;
    wD -= i_d_ * impulse * jw_d_;
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
    data.velocities[m_indexC].v = vC;
    data.velocities[m_indexC].w = wC;
    data.velocities[m_indexD].v = vD;
    data.velocities[m_indexD].w = wD;
  }
  
  bool GearJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    Vec2 cC = data.positions[m_indexC].c;
    float aC = data.positions[m_indexC].a;
    Vec2 cD = data.positions[m_indexD].c;
    float aD = data.positions[m_indexD].a;
    
    Rot qA(aA), qB(aB), qC(aC), qD(aD);
    
    float linearError = 0.0f;
    
    float coordinateA, coordinateB;
    
    Vec2 JvAC, JvBD;
    float JwA, JwB, JwC, JwD;
    float mass = 0.0f;
    
    if (type_a_ == revoluteJoint) {
      JvAC.SetZero();
      JwA = 1.0f;
      JwC = 1.0f;
      mass += m_iA + i_c_;
      
      coordinateA = aA - aC - reference_angle_A;
    }
    else {
      Vec2 u = Mul(qC, local_axis_c_);
      Vec2 rC = Mul(qC, local_anchor_c_ - lc_c_);
      Vec2 rA = Mul(qA, local_anchor_a_ - lc_a_);
      JvAC = u;
      JwC = Cross(rC, u);
      JwA = Cross(rA, u);
      mass += m_c_ + m_a_ + i_c_ * JwC * JwC + m_iA * JwA * JwA;
      
      Vec2 pC = local_anchor_c_ - lc_c_;
      Vec2 pA = MulT(qC, rA + (cA - cC));
      coordinateA = Dot(pA - pC, local_axis_c_);
    }
    
    if (type_b_ == revoluteJoint) {
      JvBD.SetZero();
      JwB = ra_tio;
      JwD = ra_tio;
      mass += ra_tio * ra_tio * (i_b_ + i_d_);
      
      coordinateB = aB - aD - reference_angle_B;
    }
    else {
      Vec2 u = Mul(qD, local_axis_d_);
      Vec2 rD = Mul(qD, local_anchor_d_ - lc_d_);
      Vec2 rB = Mul(qB, local_anchor_b_ - lc_b_);
      JvBD = ra_tio * u;
      JwD = ra_tio * Cross(rD, u);
      JwB = ra_tio * Cross(rB, u);
      mass += ra_tio * ra_tio * (m_d_ + m_b_) + i_d_ * JwD * JwD + i_b_ * JwB * JwB;
      
      Vec2 pD = local_anchor_d_ - lc_d_;
      Vec2 pB = MulT(qD, rB + (cB - cD));
      coordinateB = Dot(pB - pD, local_axis_d_);
    }
    
    float C = (coordinateA + ra_tio * coordinateB) - constant_;
    
    float impulse = 0.0f;
    if (mass > 0.0f) {
      impulse = -C / mass;
    }
    
    cA += m_a_ * impulse * JvAC;
    aA += m_iA * impulse * JwA;
    cB += m_b_ * impulse * JvBD;
    aB += i_b_ * impulse * JwB;
    cC -= m_c_ * impulse * JvAC;
    aC -= i_c_ * impulse * JwC;
    cD -= m_d_ * impulse * JvBD;
    aD -= i_d_ * impulse * JwD;
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    data.positions[m_indexC].c = cC;
    data.positions[m_indexC].a = aC;
    data.positions[m_indexD].c = cD;
    data.positions[m_indexD].a = aD;
    
    // TODO_ERIN not implemented
    return linearError < LinearSlop;
  }
  
  Vec2 GearJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 GearJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 GearJoint::GetReactionForce(float inv_dt) const {
    Vec2 P = impulse_ * jv_ac_;
    return inv_dt * P;
  }
  
  float GearJoint::GetReactionTorque(float inv_dt) const {
    float L = impulse_ * jw_a_;
    return inv_dt * L;
  }
  
  void GearJoint::SetRatio(float ratio) {
    IK_ASSERT(IsValid(ratio));
    ra_tio = ratio;
  }
  
  float GearJoint::GetRatio() const {
    return ra_tio;
  }
    
}
