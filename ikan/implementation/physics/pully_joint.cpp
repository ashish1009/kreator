//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "pully_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"

namespace physics {
 
  void PulleyJointDef::Initialize(Body* bA, Body* bB,
                                    const Vec2& groundA, const Vec2& groundB,
                                    const Vec2& anchorA, const Vec2& anchorB,
                                    float r)
  {
    bodyA = bA;
    bodyB = bB;
    groundAnchorA = groundA;
    groundAnchorB = groundB;
    localAnchorA = bodyA->GetLocalPoint(anchorA);
    localAnchorB = bodyB->GetLocalPoint(anchorB);
    Vec2 dA = anchorA - groundA;
    lengthA = dA.Length();
    Vec2 dB = anchorB - groundB;
    lengthB = dB.Length();
    ratio = r;
    IK_ASSERT(ratio > FLT_EPSILON);
  }
  
  PulleyJoint::PulleyJoint(const PulleyJointDef* def)
  : Joint(def)
  {
    m_groundAnchorA = def->groundAnchorA;
    m_groundAnchorB = def->groundAnchorB;
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    
    m_lengthA = def->lengthA;
    m_lengthB = def->lengthB;
    
    IK_ASSERT(def->ratio != 0.0f);
    m_ratio = def->ratio;
    
    m_constant = def->lengthA + m_ratio * def->lengthB;
    
    m_impulse = 0.0f;
  }
  
  void PulleyJoint::InitVelocityConstraints(const SolverData& data)
  {
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
    
    // Get the pulley axes.
    m_uA = cA + m_rA - m_groundAnchorA;
    m_uB = cB + m_rB - m_groundAnchorB;
    
    float lengthA = m_uA.Length();
    float lengthB = m_uB.Length();
    
    if (lengthA > 10.0f * LinearSlop)
    {
      m_uA *= 1.0f / lengthA;
    }
    else
    {
      m_uA.SetZero();
    }
    
    if (lengthB > 10.0f * LinearSlop)
    {
      m_uB *= 1.0f / lengthB;
    }
    else
    {
      m_uB.SetZero();
    }
    
    // Compute effective mass.
    float ruA = Cross(m_rA, m_uA);
    float ruB = Cross(m_rB, m_uB);
    
    float mA = m_invMassA + m_invIA * ruA * ruA;
    float mB = m_invMassB + m_invIB * ruB * ruB;
    
    m_mass = mA + m_ratio * m_ratio * mB;
    
    if (m_mass > 0.0f)
    {
      m_mass = 1.0f / m_mass;
    }
    
    if (data.step.warmStarting)
    {
      // Scale impulses to support variable time steps.
      m_impulse *= data.step.dtRatio;
      
      // Warm starting.
      Vec2 PA = -(m_impulse) * m_uA;
      Vec2 PB = (-m_ratio * m_impulse) * m_uB;
      
      vA += m_invMassA * PA;
      wA += m_invIA * Cross(m_rA, PA);
      vB += m_invMassB * PB;
      wB += m_invIB * Cross(m_rB, PB);
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
  
  void PulleyJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    Vec2 vpA = vA + Cross(wA, m_rA);
    Vec2 vpB = vB + Cross(wB, m_rB);
    
    float Cdot = -Dot(m_uA, vpA) - m_ratio * Dot(m_uB, vpB);
    float impulse = -m_mass * Cdot;
    m_impulse += impulse;
    
    Vec2 PA = -impulse * m_uA;
    Vec2 PB = -m_ratio * impulse * m_uB;
    vA += m_invMassA * PA;
    wA += m_invIA * Cross(m_rA, PA);
    vB += m_invMassB * PB;
    wB += m_invIB * Cross(m_rB, PB);
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  bool PulleyJoint::SolvePositionConstraints(const SolverData& data)
  {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    
    Rot qA(aA), qB(aB);
    
    Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
    Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
    
    // Get the pulley axes.
    Vec2 uA = cA + rA - m_groundAnchorA;
    Vec2 uB = cB + rB - m_groundAnchorB;
    
    float lengthA = uA.Length();
    float lengthB = uB.Length();
    
    if (lengthA > 10.0f * LinearSlop)
    {
      uA *= 1.0f / lengthA;
    }
    else
    {
      uA.SetZero();
    }
    
    if (lengthB > 10.0f * LinearSlop)
    {
      uB *= 1.0f / lengthB;
    }
    else
    {
      uB.SetZero();
    }
    
    // Compute effective mass.
    float ruA = Cross(rA, uA);
    float ruB = Cross(rB, uB);
    
    float mA = m_invMassA + m_invIA * ruA * ruA;
    float mB = m_invMassB + m_invIB * ruB * ruB;
    
    float mass = mA + m_ratio * m_ratio * mB;
    
    if (mass > 0.0f)
    {
      mass = 1.0f / mass;
    }
    
    float C = m_constant - lengthA - m_ratio * lengthB;
    float linearError = Abs(C);
    
    float impulse = -mass * C;
    
    Vec2 PA = -impulse * uA;
    Vec2 PB = -m_ratio * impulse * uB;
    
    cA += m_invMassA * PA;
    aA += m_invIA * Cross(rA, PA);
    cB += m_invMassB * PB;
    aB += m_invIB * Cross(rB, PB);
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    
    return linearError < LinearSlop;
  }
  
  Vec2 PulleyJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 PulleyJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 PulleyJoint::GetReactionForce(float inv_dt) const
  {
    Vec2 P = m_impulse * m_uB;
    return inv_dt * P;
  }
  
  float PulleyJoint::GetReactionTorque(float inv_dt) const
  {
    NOT_USED(inv_dt);
    return 0.0f;
  }
  
  Vec2 PulleyJoint::GetGroundAnchorA() const
  {
    return m_groundAnchorA;
  }
  
  Vec2 PulleyJoint::GetGroundAnchorB() const
  {
    return m_groundAnchorB;
  }
  
  float PulleyJoint::GetLengthA() const
  {
    return m_lengthA;
  }
  
  float PulleyJoint::GetLengthB() const
  {
    return m_lengthB;
  }
  
  float PulleyJoint::GetRatio() const
  {
    return m_ratio;
  }
  
  float PulleyJoint::GetCurrentLengthA() const
  {
    Vec2 p = m_bodyA->GetWorldPoint(m_localAnchorA);
    Vec2 s = m_groundAnchorA;
    Vec2 d = p - s;
    return d.Length();
  }
  
  float PulleyJoint::GetCurrentLengthB() const
  {
    Vec2 p = m_bodyB->GetWorldPoint(m_localAnchorB);
    Vec2 s = m_groundAnchorB;
    Vec2 d = p - s;
    return d.Length();
  }
  
  void PulleyJoint::ShiftOrigin(const Vec2& newOrigin)
  {
    m_groundAnchorA -= newOrigin;
    m_groundAnchorB -= newOrigin;
  }

  
}
