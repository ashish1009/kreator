//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "wheel_joint.hpp"
#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
 
  void WheelJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    localAxisA = bodyA->GetLocalVector(axis);
  }
  
  WheelJoint::WheelJoint(const WheelJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    local_x_axis_a_ = def->localAxisA;
    local_y_axis_a_ = Cross(1.0f, local_x_axis_a_);
    
    mass_ = 0.0f;
    impulse_ = 0.0f;
    motor_mass_ = 0.0f;
    motor_impulse_ = 0.0f;
    spring_mass_ = 0.0f;
    spring_impusle_ = 0.0f;
    
    axis_mass_ = 0.0f;
    lower_impulse_ = 0.0f;
    upper_impulse_ = 0.0f;
    lower_translation_ = def->lowerTranslation;
    upper_translation_ = def->upperTranslation;
    enable_limit_ = def->enableLimit;
    
    max_motor_torque_ = def->maxMotorTorque;
    motor_speed_ = def->motorSpeed;
    enable_motor_ = def->enableMotor;
    
    bias_ = 0.0f;
    gamma_ = 0.0f;
    
    a_x_.SetZero();
    a_y_.SetZero();
    
    stiffness_ = def->stiffness;
    damping_ = def->damping;
  }
  
  void WheelJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->is_land_index_;
    index_b_ = body_b_->is_land_index_;
    local_center_a_ = body_a_->sweep_.localCenter;
    local_center_b_ = body_b_->sweep_.localCenter;
    inv_mass_a_ = body_a_->inv_mass_;
    inv_mass_b_ = body_b_->inv_mass_;
    inv_i_a_ = body_a_->inv_inertia_;
    inv_i_b_ = body_b_->inv_inertia_;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    Rot qA(aA), qB(aB);
    
    // Compute the effective masses.
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    Vec2 d = cB + rB - cA - rA;
    
    // Point to line constraint
    {
      a_y_ = Mul(qA, local_y_axis_a_);
      s_a_y_ = Cross(d + rA, a_y_);
      s_b_y_ = Cross(rB, a_y_);
      
      mass_ = mA + mB + iA * s_a_y_ * s_a_y_ + iB * s_b_y_ * s_b_y_;
      
      if (mass_ > 0.0f)
      {
        mass_ = 1.0f / mass_;
      }
    }
    
    // Spring constraint
    a_x_ = Mul(qA, local_x_axis_a_);
    m_sAx = Cross(d + rA, a_x_);
    s_b_x_ = Cross(rB, a_x_);
    
    const float invMass = mA + mB + iA * m_sAx * m_sAx + iB * s_b_x_ * s_b_x_;
    if (invMass > 0.0f) {
      axis_mass_ = 1.0f / invMass;
    }
    else {
      axis_mass_ = 0.0f;
    }
    
    spring_mass_ = 0.0f;
    bias_ = 0.0f;
    gamma_ = 0.0f;
    
    if (stiffness_ > 0.0f && invMass > 0.0f) {
      spring_mass_ = 1.0f / invMass;
      
      float C = Dot(d, a_x_);
      
      // magic formulas
      float h = data.step.dt;
      gamma_ = h * (damping_ + h * stiffness_);
      if (gamma_ > 0.0f)
      {
        gamma_ = 1.0f / gamma_;
      }
      
      bias_ = C * h * stiffness_ * gamma_;
      
      spring_mass_ = invMass + gamma_;
      if (spring_mass_ > 0.0f)
      {
        spring_mass_ = 1.0f / spring_mass_;
      }
    }
    else {
      spring_impusle_ = 0.0f;
    }
    
    if (enable_limit_) {
      translation_ = Dot(a_x_, d);
    }
    else {
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    if (enable_motor_) {
      motor_mass_ = iA + iB;
      if (motor_mass_ > 0.0f)
      {
        motor_mass_ = 1.0f / motor_mass_;
      }
    }
    else {
      motor_mass_ = 0.0f;
      motor_impulse_ = 0.0f;
    }
    
    if (data.step.warm_starting) {
      // Account for variable time step.
      impulse_ *= data.step.dt_ratio;
      spring_impusle_ *= data.step.dt_ratio;
      motor_impulse_ *= data.step.dt_ratio;
      
      float axialImpulse = spring_impusle_ + lower_impulse_ - upper_impulse_;
      Vec2 P = impulse_ * a_y_ + axialImpulse * a_x_;
      float LA = impulse_ * s_a_y_ + axialImpulse * m_sAx + motor_impulse_;
      float LB = impulse_ * s_b_y_ + axialImpulse * s_b_x_ + motor_impulse_;
      
      vA -= inv_mass_a_ * P;
      wA -= inv_i_a_ * LA;
      
      vB += inv_mass_b_ * P;
      wB += inv_i_b_ * LB;
    }
    else {
      impulse_ = 0.0f;
      spring_impusle_ = 0.0f;
      motor_impulse_ = 0.0f;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void WheelJoint::SolveVelocityConstraints(const SolverData& data) {
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    // Solve spring constraint
    {
      float Cdot = Dot(a_x_, vB - vA) + s_b_x_ * wB - m_sAx * wA;
      float impulse = -spring_mass_ * (Cdot + bias_ + gamma_ * spring_impusle_);
      spring_impusle_ += impulse;
      
      Vec2 P = impulse * a_x_;
      float LA = impulse * m_sAx;
      float LB = impulse * s_b_x_;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
    }
    
    // Solve rotational motor constraint
   {
      float Cdot = wB - wA - motor_speed_;
      float impulse = -motor_mass_ * Cdot;
      
      float oldImpulse = motor_impulse_;
      float maxImpulse = data.step.dt * max_motor_torque_;
      motor_impulse_ = Clamp(motor_impulse_ + impulse, -maxImpulse, maxImpulse);
      impulse = motor_impulse_ - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    if (enable_limit_) {
      // Lower limit
      {
        float C = translation_ - lower_translation_;
        float Cdot = Dot(a_x_, vB - vA) + s_b_x_ * wB - m_sAx * wA;
        float impulse = -axis_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = lower_impulse_;
        lower_impulse_ = Max(lower_impulse_ + impulse, 0.0f);
        impulse = lower_impulse_ - oldImpulse;
        
        Vec2 P = impulse * a_x_;
        float LA = impulse * m_sAx;
        float LB = impulse * s_b_x_;
        
        vA -= mA * P;
        wA -= iA * LA;
        vB += mB * P;
        wB += iB * LB;
      }
      
      // Upper limit
      // Note: signs are flipped to keep C positive when the constraint is satisfied.
      // This also keeps the impulse positive when the limit is active.
      {
        float C = upper_translation_ - translation_;
        float Cdot = Dot(a_x_, vA - vB) + m_sAx * wA - s_b_x_ * wB;
        float impulse = -axis_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = upper_impulse_;
        upper_impulse_ = Max(upper_impulse_ + impulse, 0.0f);
        impulse = upper_impulse_ - oldImpulse;
        
        Vec2 P = impulse * a_x_;
        float LA = impulse * m_sAx;
        float LB = impulse * s_b_x_;
        
        vA += mA * P;
        wA += iA * LA;
        vB -= mB * P;
        wB -= iB * LB;
      }
    }
    
    // Solve point to line constraint
    {
      float Cdot = Dot(a_y_, vB - vA) + s_b_y_ * wB - s_a_y_ * wA;
      float impulse = -mass_ * Cdot;
      impulse_ += impulse;
      
      Vec2 P = impulse * a_y_;
      float LA = impulse * s_a_y_;
      float LB = impulse * s_b_y_;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool WheelJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    float linearError = 0.0f;
    
    if (enable_limit_) {
      Rot qA(aA), qB(aB);
      
      Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
      Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
      Vec2 d = (cB - cA) + rB - rA;
      
      Vec2 ax = Mul(qA, local_x_axis_a_);
      float sAx = Cross(d + rA, a_x_);
      float sBx = Cross(rB, a_x_);
      
      float C = 0.0f;
      float translation = Dot(ax, d);
      if (Abs(upper_translation_ - lower_translation_) < 2.0f * LinearSlop)
      {
        C = translation;
      }
      else if (translation <= lower_translation_)
      {
        C = Min(translation - lower_translation_, 0.0f);
      }
      else if (translation >= upper_translation_)
      {
        C = Max(translation - upper_translation_, 0.0f);
      }
      
      if (C != 0.0f)
      {
        
        float invMass = inv_mass_a_ + inv_mass_b_ + inv_i_a_ * sAx * sAx + inv_i_b_ * sBx * sBx;
        float impulse = 0.0f;
        if (invMass != 0.0f)
        {
          impulse = -C / invMass;
        }
        
        Vec2 P = impulse * ax;
        float LA = impulse * sAx;
        float LB = impulse * sBx;
        
        cA -= inv_mass_a_ * P;
        aA -= inv_i_a_ * LA;
        cB += inv_mass_b_ * P;
        aB += inv_i_b_ * LB;
        
        linearError = Abs(C);
      }
    }
    
    // Solve perpendicular constraint
    {
      Rot qA(aA), qB(aB);
      
      Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
      Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
      Vec2 d = (cB - cA) + rB - rA;
      
      Vec2 ay = Mul(qA, local_y_axis_a_);
      
      float sAy = Cross(d + rA, ay);
      float sBy = Cross(rB, ay);
      
      float C = Dot(d, ay);
      
      float invMass = inv_mass_a_ + inv_mass_b_ + inv_i_a_ * s_a_y_ * s_a_y_ + inv_i_b_ * s_b_y_ * s_b_y_;
      
      float impulse = 0.0f;
      if (invMass != 0.0f)
      {
        impulse = - C / invMass;
      }
      
      Vec2 P = impulse * ay;
      float LA = impulse * sAy;
      float LB = impulse * sBy;
      
      cA -= inv_mass_a_ * P;
      aA -= inv_i_a_ * LA;
      cB += inv_mass_b_ * P;
      aB += inv_i_b_ * LB;
      
      linearError = Max(linearError, Abs(C));
    }
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return linearError <= LinearSlop;
  }
  
  Vec2 WheelJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 WheelJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 WheelJoint::GetReactionForce(float inv_dt) const {
    return inv_dt * (impulse_ * a_y_ + (spring_impusle_ + lower_impulse_ - upper_impulse_) * a_x_);
  }
  
  float WheelJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * motor_impulse_;
  }
  
  float WheelJoint::GetJointTranslation() const {
    Body* bA = body_a_;
    Body* bB = body_b_;
    
    Vec2 pA = bA->GetWorldPoint(local_anchor_a_);
    Vec2 pB = bB->GetWorldPoint(local_anchor_b_);
    Vec2 d = pB - pA;
    Vec2 axis = bA->GetWorldVector(local_x_axis_a_);
    
    float translation = Dot(d, axis);
    return translation;
  }
  
  float WheelJoint::GetJointLinearSpeed() const {
    Body* bA = body_a_;
    Body* bB = body_b_;
    
    Vec2 rA = Mul(bA->xf_.q, local_anchor_a_ - bA->sweep_.localCenter);
    Vec2 rB = Mul(bB->xf_.q, local_anchor_b_ - bB->sweep_.localCenter);
    Vec2 p1 = bA->sweep_.c + rA;
    Vec2 p2 = bB->sweep_.c + rB;
    Vec2 d = p2 - p1;
    Vec2 axis = Mul(bA->xf_.q, local_x_axis_a_);
    
    Vec2 vA = bA->linear_velocity_;
    Vec2 vB = bB->linear_velocity_;
    float wA = bA->angular_velocity_;
    float wB = bB->angular_velocity_;
    
    float speed = Dot(d, Cross(wA, axis)) + Dot(axis, vB + Cross(wB, rB) - vA - Cross(wA, rA));
    return speed;
  }
  
  float WheelJoint::GetJointAngle() const {
    Body* bA = body_a_;
    Body* bB = body_b_;
    return bB->sweep_.a - bA->sweep_.a;
  }
  
  float WheelJoint::GetJointAngularSpeed() const {
    float wA = body_a_->angular_velocity_;
    float wB = body_b_->angular_velocity_;
    return wB - wA;
  }
  
  bool WheelJoint::IsLimitEnabled() const {
    return enable_limit_;
  }
  
  void WheelJoint::EnableLimit(bool flag) {
    if (flag != enable_limit_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_limit_ = flag;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
  }
  
  float WheelJoint::GetLowerLimit() const {
    return lower_translation_;
  }
  
  float WheelJoint::GetUpperLimit() const {
    return upper_translation_;
  }
  
  void WheelJoint::SetLimits(float lower, float upper) {
    IK_ASSERT(lower <= upper);
    if (lower != lower_translation_ || upper != upper_translation_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      lower_translation_ = lower;
      upper_translation_ = upper;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
  }
  
  bool WheelJoint::IsMotorEnabled() const {
    return enable_motor_;
  }
  
  void WheelJoint::EnableMotor(bool flag) {
    if (flag != enable_motor_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_motor_ = flag;
    }
  }
  
  void WheelJoint::SetMotorSpeed(float speed) {
    if (speed != motor_speed_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      motor_speed_ = speed;
    }
  }
  
  void WheelJoint::SetMaxMotorTorque(float torque) {
    if (torque != max_motor_torque_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      max_motor_torque_ = torque;
    }
  }
  
  float WheelJoint::GetMotorTorque(float inv_dt) const {
    return inv_dt * motor_impulse_;
  }
  
  void WheelJoint::SetStiffness(float stiffness) {
    stiffness_ = stiffness;
  }
  
  float WheelJoint::GetStiffness() const {
    return stiffness_;
  }
  
  void WheelJoint::SetDamping(float damping) {
    damping_ = damping;
  }
  
  float WheelJoint::GetDamping() const {
    return damping_;
  }
  
  ///
  void WheelJoint::Draw(physics::Draw* draw) const {
    const Transform& xfA = body_a_->GetTransform();
    const Transform& xfB = body_b_->GetTransform();
    Vec2 pA = Mul(xfA, local_anchor_a_);
    Vec2 pB = Mul(xfB, local_anchor_b_);
    
    Vec2 axis = Mul(xfA.q, local_x_axis_a_);
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.3f, 0.3f, 0.9f);
    Color c5(0.4f, 0.4f, 0.4f);
    
    draw->DrawSegment(pA, pB, c5);
    
    if (enable_limit_) {
      Vec2 lower = pA + lower_translation_ * axis;
      Vec2 upper = pA + upper_translation_ * axis;
      Vec2 perp = Mul(xfA.q, local_y_axis_a_);
      draw->DrawSegment(lower, upper, c1);
      draw->DrawSegment(lower - 0.5f * perp, lower + 0.5f * perp, c2);
      draw->DrawSegment(upper - 0.5f * perp, upper + 0.5f * perp, c3);
    }
    else {
      draw->DrawSegment(pA - 1.0f * axis, pA + 1.0f * axis, c1);
    }
    
    draw->DrawPoint(pA, 5.0f, c1);
    draw->DrawPoint(pB, 5.0f, c4);
  }

  float WheelJoint::GetMotorSpeed() const {
    return motor_speed_;
  }
  
  float WheelJoint::GetMaxMotorTorque() const {
    return max_motor_torque_;
  }
  

}
