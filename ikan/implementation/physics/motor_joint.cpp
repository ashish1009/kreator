//
//  motor_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "motor_joint.hpp"
#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
 
  void MotorJointDef::Initialize(Body* bA, Body* bB) {
    bodyA = bA;
    bodyB = bB;
    Vec2 xB = bodyB->GetPosition();
    linearOffset = bodyA->GetLocalPoint(xB);
    
    float angleA = bodyA->GetAngle();
    float angleB = bodyB->GetAngle();
    angularOffset = angleB - angleA;
  }
  
  MotorJoint::MotorJoint(const MotorJointDef* def)
  : Joint(def) {
    linear_offset_ = def->linearOffset;
    angular_offset_ = def->angularOffset;
    
    linear_impulse_.SetZero();
    angular_impulse_ = 0.0f;
    
    max_force_ = def->maxForce;
    max_torque_ = def->maxTorque;
    correction_factor_ = def->correctionFactor;
  }
  
  void MotorJoint::InitVelocityConstraints(const SolverData& data) {
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
    
    // Compute the effective mass matrix.
    ra_ = Mul(qA, linear_offset_ - local_center_a_);
    rb_ = Mul(qB, -local_center_b_);
    
    // J = [-I -r1_skew I r2_skew]
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
    //     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    // Upper 2 by 2 of K for point to point
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
    
    linear_error_ = cB + rb_ - cA - ra_;
    angular_error_ = aB - aA - angular_offset_;
    
    if (data.step.warm_starting) {
      // Scale impulses to support a variable time step.
      linear_impulse_ *= data.step.dt_ratio;
      angular_impulse_ *= data.step.dt_ratio;
      
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
  
  void MotorJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    float h = data.step.dt;
    float inv_h = data.step.inv_dt;
    
    // Solve angular friction
    {
      float Cdot = wB - wA + inv_h * correction_factor_ * angular_error_;
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
      Vec2 Cdot = vB + Cross(wB, rb_) - vA - Cross(wA, ra_) + inv_h * correction_factor_ * linear_error_;
      
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
  
  bool MotorJoint::SolvePositionConstraints(const SolverData& data) {
    NOT_USED(data);
    
    return true;
  }
  
  Vec2 MotorJoint::GetAnchorA() const {
    return body_a_->GetPosition();
  }
  
  Vec2 MotorJoint::GetAnchorB() const {
    return body_b_->GetPosition();
  }
  
  Vec2 MotorJoint::GetReactionForce(float inv_dt) const {
    return inv_dt * linear_impulse_;
  }
  
  float MotorJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * angular_impulse_;
  }
  
  void MotorJoint::SetMaxForce(float force) {
    IK_ASSERT(IsValid(force) && force >= 0.0f);
    max_force_ = force;
  }
  
  float MotorJoint::GetMaxForce() const {
    return max_force_;
  }
  
  void MotorJoint::SetMaxTorque(float torque) {
    IK_ASSERT(IsValid(torque) && torque >= 0.0f);
    max_torque_ = torque;
  }
  
  float MotorJoint::GetMaxTorque() const {
    return max_torque_;
  }
  
  void MotorJoint::SetCorrectionFactor(float factor) {
    IK_ASSERT(IsValid(factor) && 0.0f <= factor && factor <= 1.0f);
    correction_factor_ = factor;
  }
  
  float MotorJoint::GetCorrectionFactor() const {
    return correction_factor_;
  }
  
  void MotorJoint::SetLinearOffset(const Vec2& linearOffset) {
    if (linearOffset.x != linear_offset_.x || linearOffset.y != linear_offset_.y) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      linear_offset_ = linearOffset;
    }
  }
  
  const Vec2& MotorJoint::GetLinearOffset() const {
    return linear_offset_;
  }
  
  void MotorJoint::SetAngularOffset(float angularOffset) {
    if (angularOffset != angular_offset_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      angular_offset_ = angularOffset;
    }
  }
  
  float MotorJoint::GetAngularOffset() const {
    return angular_offset_;
  }

  
}
