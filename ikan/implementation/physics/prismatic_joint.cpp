//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "prismatic_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"

namespace physics {
  
  void PrismaticJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis) {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    localAxisA = bodyA->GetLocalVector(axis);
    referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
  }
  
  PrismaticJoint::PrismaticJoint(const PrismaticJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    local_x_axis_a_ = def->localAxisA;
    local_x_axis_a_.Normalize();
    local_y_axis_a_ = Cross(1.0f, local_x_axis_a_);
    reference_angle_ = def->referenceAngle;
    
    impulse_.SetZero();
    axial_mass_ = 0.0f;
    motor_impulse_ = 0.0f;
    lower_impulse_ = 0.0f;
    upper_impulse_ = 0.0f;
    
    lower_translation_ = def->lowerTranslation;
    upper_translation_ = def->upperTranslation;
    
    IK_ASSERT(lower_translation_ <= upper_translation_);
    
    max_motor_force_ = def->maxMotorForce;
    motor_speed_ = def->motorSpeed;
    enable_limit_ = def->enableLimit;
    enable_motor_ = def->enableMotor;
    
    translation_ = 0.0f;
    axis_.SetZero();
    perp_.SetZero();
  }
  
  void PrismaticJoint::InitVelocityConstraints(const SolverData& data) {
    index_a_ = body_a_->island_index_;
    index_b_ = body_b_->island_index_;
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
    
    // Compute the effective masses.
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    Vec2 d = (cB - cA) + rB - rA;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    // Compute motor Jacobian and effective mass.
    {
      axis_ = Mul(qA, local_x_axis_a_);
      a1_ = Cross(d + rA, axis_);
      a2_ = Cross(rB, axis_);
      
      axial_mass_ = mA + mB + iA * a1_ * a1_ + iB * a2_ * a2_;
      if (axial_mass_ > 0.0f) {
        axial_mass_ = 1.0f / axial_mass_;
      }
    }
    
    // Prismatic constraint.
   {
      perp_ = Mul(qA, local_y_axis_a_);
      
      s1_ = Cross(d + rA, perp_);
      s2_ = Cross(rB, perp_);
      
      float k11 = mA + mB + iA * s1_ * s1_ + iB * s2_ * s2_;
      float k12 = iA * s1_ + iB * s2_;
      float k22 = iA + iB;
      if (k22 == 0.0f) {
        // For bodies with fixed rotation.
        k22 = 1.0f;
      }
      
      k_.ex.Set(k11, k12);
      k_.ey.Set(k12, k22);
    }
    
    if (enable_limit_) {
      translation_ = Dot(axis_, d);
    }
    else {
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    if (enable_motor_ == false) {
      motor_impulse_ = 0.0f;
    }
    
    if (data.step.warmStarting) {
      // Account for variable time step.
      impulse_ *= data.step.dtRatio;
      motor_impulse_ *= data.step.dtRatio;
      lower_impulse_ *= data.step.dtRatio;
      upper_impulse_ *= data.step.dtRatio;
      
      float axialImpulse = motor_impulse_ + lower_impulse_ - upper_impulse_;
      Vec2 P = impulse_.x * perp_ + axialImpulse * axis_;
      float LA = impulse_.x * s1_ + impulse_.y + axialImpulse * a1_;
      float LB = impulse_.x * s2_ + impulse_.y + axialImpulse * a2_;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
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
  
  void PrismaticJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    // Solve linear motor constraint
    if (enable_motor_) {
      float Cdot = Dot(axis_, vB - vA) + a2_ * wB - a1_ * wA;
      float impulse = axial_mass_ * (motor_speed_ - Cdot);
      float oldImpulse = motor_impulse_;
      float maxImpulse = data.step.dt * max_motor_force_;
      motor_impulse_ = Clamp(motor_impulse_ + impulse, -maxImpulse, maxImpulse);
      impulse = motor_impulse_ - oldImpulse;
      
      Vec2 P = impulse * axis_;
      float LA = impulse * a1_;
      float LB = impulse * a2_;
      
      vA -= mA * P;
      wA -= iA * LA;
      vB += mB * P;
      wB += iB * LB;
    }
    
    if (enable_limit_) {
      // Lower limit
      {
        float C = translation_ - lower_translation_;
        float Cdot = Dot(axis_, vB - vA) + a2_ * wB - a1_ * wA;
        float impulse = -axial_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = lower_impulse_;
        lower_impulse_ = Max(lower_impulse_ + impulse, 0.0f);
        impulse = lower_impulse_ - oldImpulse;
        
        Vec2 P = impulse * axis_;
        float LA = impulse * a1_;
        float LB = impulse * a2_;
        
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
        float Cdot = Dot(axis_, vA - vB) + a1_ * wA - a2_ * wB;
        float impulse = -axial_mass_ * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = upper_impulse_;
        upper_impulse_ = Max(upper_impulse_ + impulse, 0.0f);
        impulse = upper_impulse_ - oldImpulse;
        
        Vec2 P = impulse * axis_;
        float LA = impulse * a1_;
        float LB = impulse * a2_;
        
        vA += mA * P;
        wA += iA * LA;
        vB -= mB * P;
        wB -= iB * LB;
      }
    }
    
    // Solve the prismatic constraint in block form.
    {
      Vec2 Cdot;
      Cdot.x = Dot(perp_, vB - vA) + s2_ * wB - s1_ * wA;
      Cdot.y = wB - wA;
      
      Vec2 df = k_.Solve(-Cdot);
      impulse_ += df;
      
      Vec2 P = df.x * perp_;
      float LA = df.x * s1_ + df.y;
      float LB = df.x * s2_ + df.y;
      
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
  
  // A velocity based solver computes reaction forces(impulses) using the velocity constraint solver.Under this context,
  // the position solver is not there to resolve forces.It is only there to cope with integration error.
  //
  // Therefore, the pseudo impulses in the position solver do not have any physical meaning.Thus it is okay if they suck.
  //
  // We could take the active state from the velocity solver.However, the joint might push past the limit when the velocity
  // solver indicates the limit is inactive.
  bool PrismaticJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    Rot qA(aA), qB(aB);
    
    float mA = inv_mass_a_, mB = inv_mass_b_;
    float iA = inv_i_a_, iB = inv_i_b_;
    
    // Compute fresh Jacobians
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    Vec2 d = cB + rB - cA - rA;
    
    Vec2 axis = Mul(qA, local_x_axis_a_);
    float a1 = Cross(d + rA, axis);
    float a2 = Cross(rB, axis);
    Vec2 perp = Mul(qA, local_y_axis_a_);
    
    float s1 = Cross(d + rA, perp);
    float s2 = Cross(rB, perp);
    
    Vec3 impulse;
    Vec2 C1;
    C1.x = Dot(perp, d);
    C1.y = aB - aA - reference_angle_;
    
    float linearError = Abs(C1.x);
    float angularError = Abs(C1.y);
    
    bool active = false;
    float C2 = 0.0f;
    if (enable_limit_) {
      float translation = Dot(axis, d);
      if (Abs(upper_translation_ - lower_translation_) < 2.0f * LinearSlop) {
        C2 = translation;
        linearError = Max(linearError, Abs(translation));
        active = true;
      }
      else if (translation <= lower_translation_) {
        C2 = Min(translation - lower_translation_, 0.0f);
        linearError = Max(linearError, lower_translation_ - translation);
        active = true;
      }
      else if (translation >= upper_translation_) {
        C2 = Max(translation - upper_translation_, 0.0f);
        linearError = Max(linearError, translation - upper_translation_);
        active = true;
      }
    }
    
    if (active) {
      float k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
      float k12 = iA * s1 + iB * s2;
      float k13 = iA * s1 * a1 + iB * s2 * a2;
      float k22 = iA + iB;
      if (k22 == 0.0f) {
        // For fixed rotation
        k22 = 1.0f;
      }
      float k23 = iA * a1 + iB * a2;
      float k33 = mA + mB + iA * a1 * a1 + iB * a2 * a2;
      
      Mat33 K;
      K.ex.Set(k11, k12, k13);
      K.ey.Set(k12, k22, k23);
      K.ez.Set(k13, k23, k33);
      
      Vec3 C;
      C.x = C1.x;
      C.y = C1.y;
      C.z = C2;
      
      impulse = K.Solve33(-C);
    }
    else {
      float k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
      float k12 = iA * s1 + iB * s2;
      float k22 = iA + iB;
      if (k22 == 0.0f) {
        k22 = 1.0f;
      }
      
      Mat22 K;
      K.ex.Set(k11, k12);
      K.ey.Set(k12, k22);
      
      Vec2 impulse1 = K.Solve(-C1);
      impulse.x = impulse1.x;
      impulse.y = impulse1.y;
      impulse.z = 0.0f;
    }
    
    Vec2 P = impulse.x * perp + impulse.z * axis;
    float LA = impulse.x * s1 + impulse.y + impulse.z * a1;
    float LB = impulse.x * s2 + impulse.y + impulse.z * a2;
    
    cA -= mA * P;
    aA -= iA * LA;
    cB += mB * P;
    aB += iB * LB;
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return linearError <= LinearSlop && angularError <= AngularSlop;
  }
  
  Vec2 PrismaticJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 PrismaticJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 PrismaticJoint::GetReactionForce(float inv_dt) const {
    return inv_dt * (impulse_.x * perp_ + (motor_impulse_ + lower_impulse_ - upper_impulse_) * axis_);
  }
  
  float PrismaticJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * impulse_.y;
  }
  
  float PrismaticJoint::GetJointTranslation() const {
    Vec2 pA = body_a_->GetWorldPoint(local_anchor_a_);
    Vec2 pB = body_b_->GetWorldPoint(local_anchor_b_);
    Vec2 d = pB - pA;
    Vec2 axis = body_a_->GetWorldVector(local_x_axis_a_);
    
    float translation = Dot(d, axis);
    return translation;
  }
  
  float PrismaticJoint::GetJointSpeed() const {
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
  
  bool PrismaticJoint::IsLimitEnabled() const {
    return enable_limit_;
  }
  
  void PrismaticJoint::EnableLimit(bool flag) {
    if (flag != enable_limit_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_limit_ = flag;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
  }
  
  float PrismaticJoint::GetLowerLimit() const {
    return lower_translation_;
  }
  
  float PrismaticJoint::GetUpperLimit() const {
    return upper_translation_;
  }
  
  void PrismaticJoint::SetLimits(float lower, float upper) {
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
  
  bool PrismaticJoint::IsMotorEnabled() const {
    return enable_motor_;
  }
  
  void PrismaticJoint::EnableMotor(bool flag) {
    if (flag != enable_motor_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      enable_motor_ = flag;
    }
  }
  
  void PrismaticJoint::SetMotorSpeed(float speed) {
    if (speed != motor_speed_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      motor_speed_ = speed;
    }
  }
  
  void PrismaticJoint::SetMaxMotorForce(float force) {
    if (force != max_motor_force_) {
      body_a_->SetAwake(true);
      body_b_->SetAwake(true);
      max_motor_force_ = force;
    }
  }
  
  float PrismaticJoint::GetMotorForce(float inv_dt) const {
    return inv_dt * motor_impulse_;
  }
  
  void PrismaticJoint::Draw(physics::Draw* draw) const {
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

  float PrismaticJoint::GetMotorSpeed() const {
    return motor_speed_;
  }
}
