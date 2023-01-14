//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "friction_joint.hpp"
#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
 
  void FrictionJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
  }
  
  FrictionJoint::FrictionJoint(const FrictionJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    
    linear_impulse_.SetZero();
    angular_impulse_ = 0.0f;
    
    max_force_ = def->maxForce;
    max_torque_ = def->maxTorque;
  }
  
  void FrictionJoint::InitVelocityConstraints(const SolverData& data) {
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
    
    // Compute the effective mass matrix.
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
    
    Mat22 K;
    K.ex.x = mA + mB + iA * ra_.y * ra_.y + iB * rb_.y * rb_.y;
    K.ex.y = -iA * ra_.x * ra_.y - iB * rb_.x * rb_.y;
    K.ey.x = K.ex.y;
    K.ey.y = mA + mB + iA * ra_.x * ra_.x + iB * rb_.x * rb_.x;
    
    linear_mass_ = K.GetInverse();
    
    angular_mass_ = iA + iB;
    if (angular_mass_ > 0.0f) {
      angular_mass_ = 1.0f / angular_mass_;
    }
    
    if (data.step.warmStarting) {
      // Scale impulses to support a variable time step.
      linear_impulse_ *= data.step.dtRatio;
      angular_impulse_ *= data.step.dtRatio;
      
      Vec2 P(linear_impulse_.x, linear_impulse_.y);
      vA -= mA * P;
      wA -= iA * (Cross(ra_, P) + angular_impulse_);
      vB += mB * P;
      wB += iB * (Cross(rb_, P) + angular_impulse_);
    }
    else {
      linear_impulse_.SetZero();
      angular_impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void FrictionJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    float h = data.step.dt;
    
    // Solve angular friction
    {
      float Cdot = wB - wA;
      float impulse = -angular_mass_ * Cdot;
      
      float oldImpulse = angular_impulse_;
      float maxImpulse = h * max_torque_;
      angular_impulse_ = Clamp(angular_impulse_ + impulse, -maxImpulse, maxImpulse);
      impulse = angular_impulse_ - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    // Solve linear friction
    {
      Vec2 Cdot = vB + Cross(wB, rb_) - vA - Cross(wA, ra_);
      
      Vec2 impulse = -Mul(linear_mass_, Cdot);
      Vec2 oldImpulse = linear_impulse_;
      linear_impulse_ += impulse;
      
      float maxImpulse = h * max_force_;
      
      if (linear_impulse_.LengthSquared() > maxImpulse * maxImpulse) {
        linear_impulse_.Normalize();
        linear_impulse_ *= maxImpulse;
      }
      
      impulse = linear_impulse_ - oldImpulse;
      
      vA -= mA * impulse;
      wA -= iA * Cross(ra_, impulse);
      
      vB += mB * impulse;
      wB += iB * Cross(rb_, impulse);
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool FrictionJoint::SolvePositionConstraints(const SolverData& data) {
    NOT_USED(data);
    
    return true;
  }
  
  Vec2 FrictionJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 FrictionJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 FrictionJoint::GetReactionForce(float inv_dt) const {
    return inv_dt * linear_impulse_;
  }
  
  float FrictionJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * angular_impulse_;
  }
  
  void FrictionJoint::SetMaxForce(float force) {
    IK_ASSERT(IsValid(force) && force >= 0.0f);
    max_force_ = force;
  }
  
  float FrictionJoint::GetMaxForce() const {
    return max_force_;
  }
  
  void FrictionJoint::SetMaxTorque(float torque) {
    IK_ASSERT(IsValid(torque) && torque >= 0.0f);
    max_torque_ = torque;
  }
  
  float FrictionJoint::GetMaxTorque() const {
    return max_torque_;
  }

  
}
