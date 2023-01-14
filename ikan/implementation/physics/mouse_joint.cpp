//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "mouse_joint.hpp"
#include "body.hpp"
#include "time_setup.hpp"

namespace physics {
  
  // p = attached point, m = mouse point
  // C = p - m
  // Cdot = v
  //      = v + cross(w, r)
  // J = [I r_skew]
  // Identity used:
  // w k % (rx i + ry j) = w * (-ry i + rx j)
  
  MouseJoint::MouseJoint(const MouseJointDef* def)
  : Joint(def) {
    target_a_ = def->target;
    local_anchor_b_ = MulT(body_b_->GetTransform(), target_a_);
    max_force_ = def->maxForce;
    stiffness_ = def->stiffness;
    damping_ = def->damping;
    
    impulse_.SetZero();
    beta_ = 0.0f;
    gamma_ = 0.0f;
  }
  
  void MouseJoint::SetTarget(const Vec2& target) {
    if (target != target_a_) {
      body_b_->SetAwake(true);
      target_a_ = target;
    }
  }
  
  const Vec2& MouseJoint::GetTarget() const {
    return target_a_;
  }
  
  void MouseJoint::SetMaxForce(float force) {
    max_force_ = force;
  }
  
  float MouseJoint::GetMaxForce() const {
    return max_force_;
  }
  
  void MouseJoint::InitVelocityConstraints(const SolverData& data) {
    index_b_ = body_b_->island_index_;
    local_center_b_ = body_b_->sweep_.localCenter;
    inv_mass_b_ = body_b_->inv_mass_;
    inv_i_b_ = body_b_->inv_inertia_;
    
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    Rot qB(aB);
    
//    float mass = body_b_->GetMass();
    
    float d = damping_;
    float k = stiffness_;
    
    // magic formulas
    // gamma has units of inverse mass.
    // beta has units of inverse time.
    float h = data.step.dt;
    gamma_ = h * (d + h * k);
    if (gamma_ != 0.0f) {
      gamma_ = 1.0f / gamma_;
    }
    beta_ = h * k * gamma_;
    
    // Compute the effective mass matrix.
    rb_ = Mul(qB, local_anchor_b_ - local_center_b_);
    
    // K    = [(1/m1 + 1/m2) * eye(2) - skew(r1) * invI1 * skew(r1) - skew(r2) * invI2 * skew(r2)]
    //      = [1/m1+1/m2     0    ] + invI1 * [r1.y*r1.y -r1.x*r1.y] + invI2 * [r1.y*r1.y -r1.x*r1.y]
    //        [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]           [-r1.x*r1.y r1.x*r1.x]
    Mat22 K;
    K.ex.x = inv_mass_b_ + inv_i_b_ * rb_.y * rb_.y + gamma_;
    K.ex.y = -inv_i_b_ * rb_.x * rb_.y;
    K.ey.x = K.ex.y;
    K.ey.y = inv_mass_b_ + inv_i_b_ * rb_.x * rb_.x + gamma_;
    
    mass_ = K.GetInverse();
    
    c_ = cB + rb_ - target_a_;
    c_ *= beta_;
    
    // Cheat with some damping
    wB *= 0.98f;
    
    if (data.step.warmStarting) {
      impulse_ *= data.step.dtRatio;
      vB += inv_mass_b_ * impulse_;
      wB += inv_i_b_ * Cross(rb_, impulse_);
    }
    else {
      impulse_.SetZero();
    }
    
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void MouseJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    // Cdot = v + cross(w, r)
    Vec2 Cdot = vB + Cross(wB, rb_);
    Vec2 impulse = Mul(mass_, -(Cdot + c_ + gamma_ * impulse_));
    
    Vec2 oldImpulse = impulse_;
    impulse_ += impulse;
    float maxImpulse = data.step.dt * max_force_;
    if (impulse_.LengthSquared() > maxImpulse * maxImpulse) {
      impulse_ *= maxImpulse / impulse_.Length();
    }
    impulse = impulse_ - oldImpulse;
    
    vB += inv_mass_b_ * impulse;
    wB += inv_i_b_ * Cross(rb_, impulse);
    
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool MouseJoint::SolvePositionConstraints(const SolverData& data) {
    NOT_USED(data);
    return true;
  }
  
  Vec2 MouseJoint::GetAnchorA() const {
    return target_a_;
  }
  
  Vec2 MouseJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 MouseJoint::GetReactionForce(float inv_dt) const {
    return inv_dt * impulse_;
  }
  
  float MouseJoint::GetReactionTorque(float inv_dt) const {
    return inv_dt * 0.0f;
  }
  
  void MouseJoint::ShiftOrigin(const Vec2& newOrigin) {
    target_a_ -= newOrigin;
  }

  
}
