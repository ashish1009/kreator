//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "weld_joint.hpp"
#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
  
  void WeldJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
  }
  
  WeldJoint::WeldJoint(const WeldJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    reference_angle_ = def->referenceAngle;
    stiffness_ = def->stiffness;
    damping_ = def->damping;
    
    impulse_.SetZero();
  }
  
  void WeldJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->is_land_index_;
    index_b_ = body_b_->is_land_index_;
    local_center_a_ = body_a_->sweep_.localCenter;
    local_center_b_ = body_b_->sweep_.localCenter;
    inv_mass_a_ = body_a_->inv_mass_;
    inv_mass_b_ = body_b_->inv_mass_;
    inv_i_a_ = body_a_->inv_inertia_;
    inv_i_b_ = body_b_->inv_inertia_;
    
    float aA = data.positions[index_a_].a;
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    
    float aB = data.positions[index_b_].a;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    Rot qA(aA), qB(aB);
    
    ra_ = Mul(qA, local_anchor_a_ - local_center_a_);
    rb_ = Mul(qB, local_anchor_b_ - local_center_b_);
    
    // J = [-I -r1_skew I r2_skew]
    //     [ 0       -1 0       1]
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
    //     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    Mat33 K;
    K.ex.x = mA + mB + ra_.y * ra_.y * iA + rb_.y * rb_.y * iB;
    K.ey.x = -ra_.y * ra_.x * iA - rb_.y * rb_.x * iB;
    K.ez.x = -ra_.y * iA - rb_.y * iB;
    K.ex.y = K.ey.x;
    K.ey.y = mA + mB + ra_.x * ra_.x * iA + rb_.x * rb_.x * iB;
    K.ez.y = ra_.x * iA + rb_.x * iB;
    K.ex.z = K.ez.x;
    K.ey.z = K.ez.y;
    K.ez.z = iA + iB;
    
    if (stiffness_ > 0.0f) {
      K.GetInverse22(&mass_);
      
      float invM = iA + iB;
      
      float C = aB - aA - reference_angle_;
      
      // Damping coefficient
      float d = damping_;
      
      // Spring stiffness
      float k = stiffness_;
      
      // magic formulas
      float h = data.step.dt;
      gamma_ = h * (d + h * k);
      gamma_ = gamma_ != 0.0f ? 1.0f / gamma_ : 0.0f;
      bias_ = C * h * k * gamma_;
      
      invM += gamma_;
      mass_.ez.z = invM != 0.0f ? 1.0f / invM : 0.0f;
    }
    else if (K.ez.z == 0.0f) {
      K.GetInverse22(&mass_);
      gamma_ = 0.0f;
      bias_ = 0.0f;
    }
    else {
      K.GetSymInverse33(&mass_);
      gamma_ = 0.0f;
      bias_ = 0.0f;
    }
    
    if (data.step.warm_starting) {
      // Scale impulses to support a variable time step.
      impulse_ *= data.step.dt_ratio;
      
      Vec2 P(impulse_.x, impulse_.y);
      
      vA -= mA * P;
      wA -= iA * (Cross(ra_, P) + impulse_.z);
      
      vB += mB * P;
      wB += iB * (Cross(rb_, P) + impulse_.z);
    }
    else {
      impulse_.SetZero();
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void WeldJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    if (stiffness_ > 0.0f) {
      float Cdot2 = wB - wA;
      
      float impulse2 = -mass_.ez.z * (Cdot2 + bias_ + gamma_ * impulse_.z);
      impulse_.z += impulse2;
      
      wA -= iA * impulse2;
      wB += iB * impulse2;
      
      Vec2 Cdot1 = vB + Cross(wB, rb_) - vA - Cross(wA, ra_);
      
      Vec2 impulse1 = -Mul22(mass_, Cdot1);
      impulse_.x += impulse1.x;
      impulse_.y += impulse1.y;
      
      Vec2 P = impulse1;
      
      vA -= mA * P;
      wA -= iA * Cross(ra_, P);
      
      vB += mB * P;
      wB += iB * Cross(rb_, P);
    }
    else {
      Vec2 Cdot1 = vB + Cross(wB, rb_) - vA - Cross(wA, ra_);
      float Cdot2 = wB - wA;
      Vec3 Cdot(Cdot1.x, Cdot1.y, Cdot2);
      
      Vec3 impulse = -Mul(mass_, Cdot);
      impulse_ += impulse;
      
      Vec2 P(impulse.x, impulse.y);
      
      vA -= mA * P;
      wA -= iA * (Cross(ra_, P) + impulse.z);
      
      vB += mB * P;
      wB += iB * (Cross(rb_, P) + impulse.z);
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool WeldJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    Rot qA(aA), qB(aB);
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    
    float positionError, angularError;
    
    Mat33 K;
    K.ex.x = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
    K.ey.x = -rA.y * rA.x * iA - rB.y * rB.x * iB;
    K.ez.x = -rA.y * iA - rB.y * iB;
    K.ex.y = K.ey.x;
    K.ey.y = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;
    K.ez.y = rA.x * iA + rB.x * iB;
    K.ex.z = K.ez.x;
    K.ey.z = K.ez.y;
    K.ez.z = iA + iB;
    
    if (stiffness_ > 0.0f) {
      Vec2 C1 =  cB + rB - cA - rA;
      
      positionError = C1.Length();
      angularError = 0.0f;
      
      Vec2 P = -K.Solve22(C1);
      
      cA -= mA * P;
      aA -= iA * Cross(rA, P);
      
      cB += mB * P;
      aB += iB * Cross(rB, P);
    }
    else {
      Vec2 C1 =  cB + rB - cA - rA;
      float C2 = aB - aA - reference_angle_;
      
      positionError = C1.Length();
      angularError = Abs(C2);
      
      Vec3 C(C1.x, C1.y, C2);
      
      Vec3 impulse;
      if (K.ez.z > 0.0f) {
        impulse = -K.Solve33(C);
      }
      else {
        Vec2 impulse2 = -K.Solve22(C1);
        impulse.Set(impulse2.x, impulse2.y, 0.0f);
      }
      
      Vec2 P(impulse.x, impulse.y);
      
      cA -= mA * P;
      aA -= iA * (Cross(rA, P) + impulse.z);
      
      cB += mB * P;
      aB += iB * (Cross(rB, P) + impulse.z);
    }
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return positionError <= LinearSlop && angularError <= AngularSlop;
  }
  
  Vec2 WeldJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 WeldJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 WeldJoint::GetReactionForce(float inv_dt) const {
    Vec2 P(impulse_.x, impulse_.y);
    return inv_dt * P;
  }
  
  float WeldJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * impulse_.z;
  }

  
}
