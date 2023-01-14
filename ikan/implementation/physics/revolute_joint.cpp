//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "revolute_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"

namespace physics {
  
  void RevoluteJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
  }
  
  RevoluteJoint::RevoluteJoint(const RevoluteJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    reference_angle_ = def->referenceAngle;
    
    impulse_.SetZero();
    axial_mass_ = 0.0f;
    motor_impulse_ = 0.0f;
    lower_impulse_ = 0.0f;
    upper_impulse_ = 0.0f;
    
    m_lowerAngle = def->lowerAngle;
    u_pperAngle = def->upperAngle;
    max_motor_torque_ = def->maxMotorTorque;
    motor_speed_ = def->motorSpeed;
    enable_limit_ = def->enableLimit;
    enable_motor_ = def->enableMotor;
    
    angle_ = 0.0f;
  }
  
  void RevoluteJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->island_index_;
    index_b_ = body_b_->island_index_;
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
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB]
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    k_.ex.x = mA + mB + ra_.y * ra_.y * iA + rb_.y * rb_.y * iB;
    k_.ey.x = -ra_.y * ra_.x * iA - rb_.y * rb_.x * iB;
    k_.ex.y = k_.ey.x;
    k_.ey.y = mA + mB + ra_.x * ra_.x * iA + rb_.x * rb_.x * iB;
    
    axial_mass_ = iA + iB;
    bool fixedRotation;
    if (axial_mass_ > 0.0f) {
      axial_mass_ = 1.0f / axial_mass_;
      fixedRotation = false;
    }
    else {
      fixedRotation = true;
    }
    
    angle_ = aB - aA - reference_angle_;
    if (enable_limit_ == false || fixedRotation) {
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    if (enable_motor_ == false || fixedRotation) {
      motor_impulse_ = 0.0f;
    }
    
    if (data.step.warmStarting) {
      // Scale impulses to support a variable time step.
      impulse_ *= data.step.dtRatio;
      motor_impulse_ *= data.step.dtRatio;
      lower_impulse_ *= data.step.dtRatio;
      upper_impulse_ *= data.step.dtRatio;
      
      float axialImpulse = motor_impulse_ + lower_impulse_ - upper_impulse_;
      Vec2 P(impulse_.x, impulse_.y);
      
      vA -= mA * P;
      wA -= iA * (Cross(ra_, P) + axialImpulse);
      
      vB += mB * P;
      wB += iB * (Cross(rb_, P) + axialImpulse);
    }
    else {
      impulse_.SetZero();
      motor_impulse_ = 0.0f;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void RevoluteJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    bool fixedRotation = (iA + iB == 0.0f);
    
    // Solve motor constraint.
    if (enable_motor_ && fixedRotation == false) {
      float Cdot = wB - wA - motor_speed_;
      float impulse = -axial_mass_ * Cdot;
      float oldImpulse = motor_impulse_;
      float maxImpulse = data.step.dt * max_motor_torque_;
      motor_impulse_ = Clamp(motor_impulse_ + impulse, -maxImpulse, maxImpulse);
      impulse = motor_impulse_ - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    if (enable_limit_ && fixedRotation == false) {
      // Lower limit
      {
        float C = angle_ - m_lowerAngle;
        float Cdot = wB - wA;
        float impulse = -axial_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = lower_impulse_;
        lower_impulse_ = Max(lower_impulse_ + impulse, 0.0f);
        impulse = lower_impulse_ - oldImpulse;
        
        wA -= iA * impulse;
        wB += iB * impulse;
      }
      
      // Upper limit
      // Note: signs are flipped to keep C positive when the constraint is satisfied.
      // This also keeps the impulse positive when the limit is active.
      {
        float C = u_pperAngle - angle_;
        float Cdot = wA - wB;
        float impulse = -axial_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = upper_impulse_;
        upper_impulse_ = Max(upper_impulse_ + impulse, 0.0f);
        impulse = upper_impulse_ - oldImpulse;
        
        wA += iA * impulse;
        wB -= iB * impulse;
      }
    }
    
    // Solve point-to-point constraint
    {
      Vec2 Cdot = vB + Cross(wB, rb_) - vA - Cross(wA, ra_);
      Vec2 impulse = k_.Solve(-Cdot);
      
      impulse_.x += impulse.x;
      impulse_.y += impulse.y;
      
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
  
  bool RevoluteJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    Rot qA(aA), qB(aB);
    
    float angularError = 0.0f;
    float positionError = 0.0f;
    
    bool fixedRotation = (inv_i_a_ + inv_i_b_ == 0.0f);
    
    // Solve angular limit constraint
    if (enable_limit_ && fixedRotation == false) {
      float angle = aB - aA - reference_angle_;
      float C = 0.0f;
      
      if (Abs(u_pperAngle - m_lowerAngle) < 2.0f * AngularSlop)
      {
        // Prevent large angular corrections
        C = Clamp(angle - m_lowerAngle, -MaxAngularCorrection, MaxAngularCorrection);
      }
      else if (angle <= m_lowerAngle)
      {
        // Prevent large angular corrections and allow some slop.
        C = Clamp(angle - m_lowerAngle + AngularSlop, - MaxAngularCorrection, 0.0f);
      }
      else if (angle >= u_pperAngle)
      {
        // Prevent large angular corrections and allow some slop.
        C = Clamp(angle - u_pperAngle - AngularSlop, 0.0f, MaxAngularCorrection);
      }
      
      float limitImpulse = -axial_mass_ * C;
      aA -= inv_i_a_ * limitImpulse;
      aB += inv_i_b_ * limitImpulse;
      angularError = Abs(C);
    }
    
    // Solve point-to-point constraint.
    {
      qA.Set(aA);
      qB.Set(aB);
      Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
      Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
      
      Vec2 C = cB + rB - cA - rA;
      positionError = C.Length();
      
      float mA = inv_mass_a_, mB = inv_mass_b_;
      float iA = inv_i_a_, iB = inv_i_b_;
      
      Mat22 K;
      K.ex.x = mA + mB + iA * rA.y * rA.y + iB * rB.y * rB.y;
      K.ex.y = -iA * rA.x * rA.y - iB * rB.x * rB.y;
      K.ey.x = K.ex.y;
      K.ey.y = mA + mB + iA * rA.x * rA.x + iB * rB.x * rB.x;
      
      Vec2 impulse = -K.Solve(C);
      
      cA -= mA * impulse;
      aA -= iA * Cross(rA, impulse);
      
      cB += mB * impulse;
      aB += iB * Cross(rB, impulse);
    }
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return positionError <= LinearSlop && angularError <= AngularSlop;
  }
  
  Vec2 RevoluteJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 RevoluteJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 RevoluteJoint::GetReactionForce(float inv_dt) const {
    Vec2 P(impulse_.x, impulse_.y);
    return inv_dt * P;
  }
  
  float RevoluteJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * (motor_impulse_ + lower_impulse_ - upper_impulse_);
  }
  
  float RevoluteJoint::GetJointAngle() const {
    Body* bA = body_a_;
    Body* bB = body_b_;
    return bB->sweep_.a - bA->sweep_.a - reference_angle_;
  }
  
  float RevoluteJoint::GetJointSpeed() const {
    Body* bA = body_a_;
    Body* bB = body_b_;
    return bB->angular_velocity_ - bA->angular_velocity_;
  }
  
  bool RevoluteJoint::IsMotorEnabled() const {
    return enable_motor_;
  }
  
  void RevoluteJoint::EnableMotor(bool flag) {
    if (flag != enable_motor_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_motor_ = flag;
    }
  }
  
  float RevoluteJoint::GetMotorTorque(float inv_dt) const {
    return inv_dt * motor_impulse_;
  }
  
  void RevoluteJoint::SetMotorSpeed(float speed) {
    if (speed != motor_speed_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      motor_speed_ = speed;
    }
  }
  
  void RevoluteJoint::SetMaxMotorTorque(float torque) {
    if (torque != max_motor_torque_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      max_motor_torque_ = torque;
    }
  }
  
  bool RevoluteJoint::IsLimitEnabled() const {
    return enable_limit_;
  }
  
  void RevoluteJoint::EnableLimit(bool flag) {
    if (flag != enable_limit_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_limit_ = flag;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
  }
  
  float RevoluteJoint::GetLowerLimit() const {
    return m_lowerAngle;
  }
  
  float RevoluteJoint::GetUpperLimit() const {
    return u_pperAngle;
  }
  
  void RevoluteJoint::SetLimits(float lower, float upper) {
    IK_ASSERT(lower <= upper);
    
    if (lower != m_lowerAngle || upper != u_pperAngle) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
      m_lowerAngle = lower;
      u_pperAngle = upper;
    }
  }
  
  void RevoluteJoint::Draw(physics::Draw* draw) const {
    const Transform& xfA = body_a_->GetTransform();
    const Transform& xfB = body_b_->GetTransform();
    Vec2 pA = Mul(xfA, local_anchor_a_);
    Vec2 pB = Mul(xfB, local_anchor_b_);
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.3f, 0.3f, 0.9f);
    Color c5(0.4f, 0.4f, 0.4f);
    
    draw->DrawPoint(pA, 5.0f, c4);
    draw->DrawPoint(pB, 5.0f, c5);
    
    float aA = body_a_->GetAngle();
    float aB = body_b_->GetAngle();
    float angle = aB - aA - reference_angle_;
    
    const float L = 0.5f;
    
    Vec2 r = L * Vec2(cosf(angle), sinf(angle));
    draw->DrawSegment(pB, pB + r, c1);
    draw->DrawCircle(pB, L, c1);
    
    if (enable_limit_) {
      Vec2 rlo = L * Vec2(cosf(m_lowerAngle), sinf(m_lowerAngle));
      Vec2 rhi = L * Vec2(cosf(u_pperAngle), sinf(u_pperAngle));
      
      draw->DrawSegment(pB, pB + rlo, c2);
      draw->DrawSegment(pB, pB + rhi, c3);
    }
    
    Color color(0.5f, 0.8f, 0.8f);
    draw->DrawSegment(xfA.p, pA, color);
    draw->DrawSegment(pA, pB, color);
    draw->DrawSegment(xfB.p, pB, color);
  }

  float RevoluteJoint::GetMotorSpeed() const {
    return motor_speed_;
  }

}
