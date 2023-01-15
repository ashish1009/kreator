//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "distance_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"

namespace physics {
  
  void DistanceJointDef::Initialize(Body* b1, Body* b2,
                                    const Vec2& anchor1, const Vec2& anchor2) {
    bodyA = b1;
    bodyB = b2;
    localAnchorA = bodyA->GetLocalPoint(anchor1);
    localAnchorB = bodyB->GetLocalPoint(anchor2);
    Vec2 d = anchor2 - anchor1;
    length = Max(d.Length(), LinearSlop);
    minLength = length;
    maxLength = length;
  }
  
  DistanceJoint::DistanceJoint(const DistanceJointDef* def)
  : Joint(def) {
    local_anchor_a_ = def->localAnchorA;
    local_anchor_b_ = def->localAnchorB;
    length_ = Max(def->length, LinearSlop);
    min_length_ = Max(def->minLength, LinearSlop);
    max_length_ = Max(def->maxLength, min_length_);
    stiffness_ = def->stiffness;
    damping_ = def->damping;
    
    gamma_ = 0.0f;
    bias_ = 0.0f;
    impulse_ = 0.0f;
    lower_impulse_ = 0.0f;
    upper_impulse_ = 0.0f;
    current_length_ = 0.0f;
  }
  
  void DistanceJoint::InitVelocityConstraints(const SolverData& data) {
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
    u_ = cB + rb_ - cA - ra_;
    
    // Handle singularity.
    current_length_ = u_.Length();
    if (current_length_ > LinearSlop) {
      u_ *= 1.0f / current_length_;
    } else {
      u_.Set(0.0f, 0.0f);
      mass_ = 0.0f;
      impulse_ = 0.0f;
      lower_impulse_ = 0.0f;
      upper_impulse_ = 0.0f;
    }
    
    float crAu = Cross(ra_, u_);
    float crBu = Cross(rb_, u_);
    float invMass = inv_mass_a_ + inv_i_a_ * crAu * crAu + inv_mass_b_ + inv_i_b_ * crBu * crBu;
    mass_ = invMass != 0.0f ? 1.0f / invMass : 0.0f;
    
    if (stiffness_ > 0.0f && min_length_ < max_length_) {
      // soft
      float C = current_length_ - length_;
      
      float d = damping_;
      float k = stiffness_;
      
      // magic formulas
      float h = data.step.dt;
      
      // gamma = 1 / (h * (d + h * k))
      // the extra factor of h in the denominator is since the lambda is an impulse, not a force
      gamma_ = h * (d + h * k);
      gamma_ = gamma_ != 0.0f ? 1.0f / gamma_ : 0.0f;
      bias_ = C * h * k * gamma_;
      
      invMass += gamma_;
      soft_mass_ = invMass != 0.0f ? 1.0f / invMass : 0.0f;
    }
    else {
      // rigid
      gamma_ = 0.0f;
      bias_ = 0.0f;
      soft_mass_ = mass_;
    }
    
    if (data.step.warm_starting) {
      // Scale the impulse to support a variable time step.
      impulse_ *= data.step.dt_ratio;
      lower_impulse_ *= data.step.dt_ratio;
      upper_impulse_ *= data.step.dt_ratio;
      
      Vec2 P = (impulse_ + lower_impulse_ - upper_impulse_) * u_;
      vA -= inv_mass_a_ * P;
      wA -= inv_i_a_ * Cross(ra_, P);
      vB += inv_mass_b_ * P;
      wB += inv_i_b_ * Cross(rb_, P);
    }
    else {
      impulse_ = 0.0f;
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  void DistanceJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[index_a_].v;
    float wA = data.velocities[index_a_].w;
    Vec2 vB = data.velocities[index_b_].v;
    float wB = data.velocities[index_b_].w;
    
    if (min_length_ < max_length_) {
      if (stiffness_ > 0.0f)
      {
        // Cdot = dot(u, v + cross(w, r))
        Vec2 vpA = vA + Cross(wA, ra_);
        Vec2 vpB = vB + Cross(wB, rb_);
        float Cdot = Dot(u_, vpB - vpA);
        
        float impulse = -soft_mass_ * (Cdot + bias_ + gamma_ * impulse_);
        impulse_ += impulse;
        
        Vec2 P = impulse * u_;
        vA -= inv_mass_a_ * P;
        wA -= inv_i_a_ * Cross(ra_, P);
        vB += inv_mass_b_ * P;
        wB += inv_i_b_ * Cross(rb_, P);
      }
      
      // lower
      {
        float C = current_length_ - min_length_;
        float bias = Max(0.0f, C) * data.step.inv_dt;
        
        Vec2 vpA = vA + Cross(wA, ra_);
        Vec2 vpB = vB + Cross(wB, rb_);
        float Cdot = Dot(u_, vpB - vpA);
        
        float impulse = -mass_ * (Cdot + bias);
        float oldImpulse = lower_impulse_;
        lower_impulse_ = Max(0.0f, lower_impulse_ + impulse);
        impulse = lower_impulse_ - oldImpulse;
        Vec2 P = impulse * u_;
        
        vA -= inv_mass_a_ * P;
        wA -= inv_i_a_ * Cross(ra_, P);
        vB += inv_mass_b_ * P;
        wB += inv_i_b_ * Cross(rb_, P);
      }
      
      // upper
      {
        float C = max_length_ - current_length_;
        float bias = Max(0.0f, C) * data.step.inv_dt;
        
        Vec2 vpA = vA + Cross(wA, ra_);
        Vec2 vpB = vB + Cross(wB, rb_);
        float Cdot = Dot(u_, vpA - vpB);
        
        float impulse = -mass_ * (Cdot + bias);
        float oldImpulse = upper_impulse_;
        upper_impulse_ = Max(0.0f, upper_impulse_ + impulse);
        impulse = upper_impulse_ - oldImpulse;
        Vec2 P = -impulse * u_;
        
        vA -= inv_mass_a_ * P;
        wA -= inv_i_a_ * Cross(ra_, P);
        vB += inv_mass_b_ * P;
        wB += inv_i_b_ * Cross(rb_, P);
      }
    }
    else {
      // Equal limits
      
      // Cdot = dot(u, v + cross(w, r))
      Vec2 vpA = vA + Cross(wA, ra_);
      Vec2 vpB = vB + Cross(wB, rb_);
      float Cdot = Dot(u_, vpB - vpA);
      
      float impulse = -mass_ * Cdot;
      impulse_ += impulse;
      
      Vec2 P = impulse * u_;
      vA -= inv_mass_a_ * P;
      wA -= inv_i_a_ * Cross(ra_, P);
      vB += inv_mass_b_ * P;
      wB += inv_i_b_ * Cross(rb_, P);
    }
    
    data.velocities[index_a_].v = vA;
    data.velocities[index_a_].w = wA;
    data.velocities[index_b_].v = vB;
    data.velocities[index_b_].w = wB;
  }
  
  bool DistanceJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[index_a_].c;
    float aA = data.positions[index_a_].a;
    Vec2 cB = data.positions[index_b_].c;
    float aB = data.positions[index_b_].a;
    
    Rot qA(aA), qB(aB);
    
    Vec2 rA = Mul(qA, local_anchor_a_ - local_center_a_);
    Vec2 rB = Mul(qB, local_anchor_b_ - local_center_b_);
    Vec2 u = cB + rB - cA - rA;
    
    float length = u.Normalize();
    float C;
    if (min_length_ == max_length_) {
      C = length - min_length_;
    }
    else if (length < min_length_) {
      C = length - min_length_;
    }
    else if (max_length_ < length) {
      C = length - max_length_;
    }
    else {
      return true;
    }
    
    float impulse = -mass_ * C;
    Vec2 P = impulse * u;
    
    cA -= inv_mass_a_ * P;
    aA -= inv_i_a_ * Cross(rA, P);
    cB += inv_mass_b_ * P;
    aB += inv_i_b_ * Cross(rB, P);
    
    data.positions[index_a_].c = cA;
    data.positions[index_a_].a = aA;
    data.positions[index_b_].c = cB;
    data.positions[index_b_].a = aB;
    
    return Abs(C) < LinearSlop;
  }
  
  Vec2 DistanceJoint::GetAnchorA() const {
    return body_a_->GetWorldPoint(local_anchor_a_);
  }
  
  Vec2 DistanceJoint::GetAnchorB() const {
    return body_b_->GetWorldPoint(local_anchor_b_);
  }
  
  Vec2 DistanceJoint::GetReactionForce(float inv_dt) const {
    Vec2 F = inv_dt * (impulse_ + lower_impulse_ - upper_impulse_) * u_;
    return F;
  }
  
  float DistanceJoint::GetReactionTorque(float inv_dt) const {
    NOT_USED(inv_dt);
    return 0.0f;
  }
  
  float DistanceJoint::SetLength(float length) {
    impulse_ = 0.0f;
    length_ = Max(LinearSlop, length);
    return length_;
  }
  
  float DistanceJoint::SetMinLength(float minLength) {
    lower_impulse_ = 0.0f;
    min_length_ = Clamp(minLength, LinearSlop, max_length_);
    return min_length_;
  }
  
  float DistanceJoint::SetMaxLength(float maxLength) {
    upper_impulse_ = 0.0f;
    max_length_ = Max(maxLength, min_length_);
    return max_length_;
  }
  
  float DistanceJoint::GetCurrentLength() const {
    Vec2 pA = body_a_->GetWorldPoint(local_anchor_a_);
    Vec2 pB = body_b_->GetWorldPoint(local_anchor_b_);
    Vec2 d = pB - pA;
    float length = d.Length();
    return length;
  }
    
  void DistanceJoint::Draw(physics::Draw* draw) const {
    const Transform& xfA = body_a_->GetTransform();
    const Transform& xfB = body_b_->GetTransform();
    Vec2 pA = Mul(xfA, local_anchor_a_);
    Vec2 pB = Mul(xfB, local_anchor_b_);
    
    Vec2 axis = pB - pA;
    float length = axis.Normalize();
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.4f, 0.4f, 0.4f);
    
    draw->DrawSegment(pA, pB, c4);
    
    Vec2 pRest = pA + length_ * axis;
    draw->DrawPoint(pRest, 8.0f, c1);
    
    if (min_length_ != max_length_) {
      if (min_length_ > LinearSlop) {
        Vec2 pMin = pA + min_length_ * axis;
        draw->DrawPoint(pMin, 4.0f, c2);
      }
      
      if (max_length_ < FLT_MAX) {
        Vec2 pMax = pA + max_length_ * axis;
        draw->DrawPoint(pMax, 4.0f, c3);
      }
    }
  }

  
}
