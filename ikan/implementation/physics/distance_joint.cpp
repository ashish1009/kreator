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
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    m_length = Max(def->length, LinearSlop);
    m_minLength = Max(def->minLength, LinearSlop);
    m_maxLength = Max(def->maxLength, m_minLength);
    m_stiffness = def->stiffness;
    m_damping = def->damping;
    
    m_gamma = 0.0f;
    m_bias = 0.0f;
    m_impulse = 0.0f;
    m_lowerImpulse = 0.0f;
    m_upperImpulse = 0.0f;
    m_currentLength = 0.0f;
  }
  
  void DistanceJoint::InitVelocityConstraints(const SolverData& data) {
    m_indexA = m_bodyA->m_islandIndex;
    m_indexB = m_bodyB->m_islandIndex;
    m_localCenterA = m_bodyA->m_sweep.localCenter;
    m_localCenterB = m_bodyB->m_sweep.localCenter;
    m_invMassA = m_bodyA->m_invMass;
    m_invMassB = m_bodyB->m_invMass;
    m_invIA = m_bodyA->m_invI;
    m_invIB = m_bodyB->m_invI;
    
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    Rot qA(aA), qB(aB);
    
    m_rA = Mul(qA, m_localAnchorA - m_localCenterA);
    m_rB = Mul(qB, m_localAnchorB - m_localCenterB);
    m_u = cB + m_rB - cA - m_rA;
    
    // Handle singularity.
    m_currentLength = m_u.Length();
    if (m_currentLength > LinearSlop) {
      m_u *= 1.0f / m_currentLength;
    } else {
      m_u.Set(0.0f, 0.0f);
      m_mass = 0.0f;
      m_impulse = 0.0f;
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    float crAu = Cross(m_rA, m_u);
    float crBu = Cross(m_rB, m_u);
    float invMass = m_invMassA + m_invIA * crAu * crAu + m_invMassB + m_invIB * crBu * crBu;
    m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;
    
    if (m_stiffness > 0.0f && m_minLength < m_maxLength)
    {
      // soft
      float C = m_currentLength - m_length;
      
      float d = m_damping;
      float k = m_stiffness;
      
      // magic formulas
      float h = data.step.dt;
      
      // gamma = 1 / (h * (d + h * k))
      // the extra factor of h in the denominator is since the lambda is an impulse, not a force
      m_gamma = h * (d + h * k);
      m_gamma = m_gamma != 0.0f ? 1.0f / m_gamma : 0.0f;
      m_bias = C * h * k * m_gamma;
      
      invMass += m_gamma;
      m_softMass = invMass != 0.0f ? 1.0f / invMass : 0.0f;
    }
    else
    {
      // rigid
      m_gamma = 0.0f;
      m_bias = 0.0f;
      m_softMass = m_mass;
    }
    
    if (data.step.warmStarting)
    {
      // Scale the impulse to support a variable time step.
      m_impulse *= data.step.dtRatio;
      m_lowerImpulse *= data.step.dtRatio;
      m_upperImpulse *= data.step.dtRatio;
      
      Vec2 P = (m_impulse + m_lowerImpulse - m_upperImpulse) * m_u;
      vA -= m_invMassA * P;
      wA -= m_invIA * Cross(m_rA, P);
      vB += m_invMassB * P;
      wB += m_invIB * Cross(m_rB, P);
    }
    else
    {
      m_impulse = 0.0f;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  void DistanceJoint::SolveVelocityConstraints(const SolverData& data) {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    if (m_minLength < m_maxLength)
    {
      if (m_stiffness > 0.0f)
      {
        // Cdot = dot(u, v + cross(w, r))
        Vec2 vpA = vA + Cross(wA, m_rA);
        Vec2 vpB = vB + Cross(wB, m_rB);
        float Cdot = Dot(m_u, vpB - vpA);
        
        float impulse = -m_softMass * (Cdot + m_bias + m_gamma * m_impulse);
        m_impulse += impulse;
        
        Vec2 P = impulse * m_u;
        vA -= m_invMassA * P;
        wA -= m_invIA * Cross(m_rA, P);
        vB += m_invMassB * P;
        wB += m_invIB * Cross(m_rB, P);
      }
      
      // lower
      {
        float C = m_currentLength - m_minLength;
        float bias = Max(0.0f, C) * data.step.inv_dt;
        
        Vec2 vpA = vA + Cross(wA, m_rA);
        Vec2 vpB = vB + Cross(wB, m_rB);
        float Cdot = Dot(m_u, vpB - vpA);
        
        float impulse = -m_mass * (Cdot + bias);
        float oldImpulse = m_lowerImpulse;
        m_lowerImpulse = Max(0.0f, m_lowerImpulse + impulse);
        impulse = m_lowerImpulse - oldImpulse;
        Vec2 P = impulse * m_u;
        
        vA -= m_invMassA * P;
        wA -= m_invIA * Cross(m_rA, P);
        vB += m_invMassB * P;
        wB += m_invIB * Cross(m_rB, P);
      }
      
      // upper
      {
        float C = m_maxLength - m_currentLength;
        float bias = Max(0.0f, C) * data.step.inv_dt;
        
        Vec2 vpA = vA + Cross(wA, m_rA);
        Vec2 vpB = vB + Cross(wB, m_rB);
        float Cdot = Dot(m_u, vpA - vpB);
        
        float impulse = -m_mass * (Cdot + bias);
        float oldImpulse = m_upperImpulse;
        m_upperImpulse = Max(0.0f, m_upperImpulse + impulse);
        impulse = m_upperImpulse - oldImpulse;
        Vec2 P = -impulse * m_u;
        
        vA -= m_invMassA * P;
        wA -= m_invIA * Cross(m_rA, P);
        vB += m_invMassB * P;
        wB += m_invIB * Cross(m_rB, P);
      }
    }
    else
    {
      // Equal limits
      
      // Cdot = dot(u, v + cross(w, r))
      Vec2 vpA = vA + Cross(wA, m_rA);
      Vec2 vpB = vB + Cross(wB, m_rB);
      float Cdot = Dot(m_u, vpB - vpA);
      
      float impulse = -m_mass * Cdot;
      m_impulse += impulse;
      
      Vec2 P = impulse * m_u;
      vA -= m_invMassA * P;
      wA -= m_invIA * Cross(m_rA, P);
      vB += m_invMassB * P;
      wB += m_invIB * Cross(m_rB, P);
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  bool DistanceJoint::SolvePositionConstraints(const SolverData& data) {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    
    Rot qA(aA), qB(aB);
    
    Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
    Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
    Vec2 u = cB + rB - cA - rA;
    
    float length = u.Normalize();
    float C;
    if (m_minLength == m_maxLength)
    {
      C = length - m_minLength;
    }
    else if (length < m_minLength)
    {
      C = length - m_minLength;
    }
    else if (m_maxLength < length)
    {
      C = length - m_maxLength;
    }
    else
    {
      return true;
    }
    
    float impulse = -m_mass * C;
    Vec2 P = impulse * u;
    
    cA -= m_invMassA * P;
    aA -= m_invIA * Cross(rA, P);
    cB += m_invMassB * P;
    aB += m_invIB * Cross(rB, P);
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    
    return Abs(C) < LinearSlop;
  }
  
  Vec2 DistanceJoint::GetAnchorA() const {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 DistanceJoint::GetAnchorB() const {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 DistanceJoint::GetReactionForce(float inv_dt) const {
    Vec2 F = inv_dt * (m_impulse + m_lowerImpulse - m_upperImpulse) * m_u;
    return F;
  }
  
  float DistanceJoint::GetReactionTorque(float inv_dt) const {
    NOT_USED(inv_dt);
    return 0.0f;
  }
  
  float DistanceJoint::SetLength(float length) {
    m_impulse = 0.0f;
    m_length = Max(LinearSlop, length);
    return m_length;
  }
  
  float DistanceJoint::SetMinLength(float minLength) {
    m_lowerImpulse = 0.0f;
    m_minLength = Clamp(minLength, LinearSlop, m_maxLength);
    return m_minLength;
  }
  
  float DistanceJoint::SetMaxLength(float maxLength) {
    m_upperImpulse = 0.0f;
    m_maxLength = Max(maxLength, m_minLength);
    return m_maxLength;
  }
  
  float DistanceJoint::GetCurrentLength() const {
    Vec2 pA = m_bodyA->GetWorldPoint(m_localAnchorA);
    Vec2 pB = m_bodyB->GetWorldPoint(m_localAnchorB);
    Vec2 d = pB - pA;
    float length = d.Length();
    return length;
  }
    
  void DistanceJoint::Draw(physics::Draw* draw) const {
    const Transform& xfA = m_bodyA->GetTransform();
    const Transform& xfB = m_bodyB->GetTransform();
    Vec2 pA = Mul(xfA, m_localAnchorA);
    Vec2 pB = Mul(xfB, m_localAnchorB);
    
    Vec2 axis = pB - pA;
    float length = axis.Normalize();
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.4f, 0.4f, 0.4f);
    
    draw->DrawSegment(pA, pB, c4);
    
    Vec2 pRest = pA + m_length * axis;
    draw->DrawPoint(pRest, 8.0f, c1);
    
    if (m_minLength != m_maxLength) {
      if (m_minLength > LinearSlop) {
        Vec2 pMin = pA + m_minLength * axis;
        draw->DrawPoint(pMin, 4.0f, c2);
      }
      
      if (m_maxLength < FLT_MAX) {
        Vec2 pMax = pA + m_maxLength * axis;
        draw->DrawPoint(pMax, 4.0f, c3);
      }
    }
  }

  
}
