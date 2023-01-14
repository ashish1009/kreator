//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
  
  GearJoint::GearJoint(const GearJointDef* def)
  : Joint(def)
  {
    m_joint1 = def->joint1;
    m_joint2 = def->joint2;
    
    m_typeA = m_joint1->GetType();
    m_typeB = m_joint2->GetType();
    
    IK_ASSERT(m_typeA == revoluteJoint || m_typeA == prismaticJoint);
    IK_ASSERT(m_typeB == revoluteJoint || m_typeB == prismaticJoint);
    
    float coordinateA, coordinateB;
    
    // TODO_ERIN there might be some problem with the joint edges in Joint.
    
    m_bodyC = m_joint1->GetBodyA();
    m_bodyA = m_joint1->GetBodyB();
    
    // Body B on joint1 must be dynamic
    IK_ASSERT(m_bodyA->type_ == BodyType::DynamicBody);
    
    // Get geometry of joint1
    Transform xfA = m_bodyA->m_xf;
    float aA = m_bodyA->m_sweep.a;
    Transform xfC = m_bodyC->m_xf;
    float aC = m_bodyC->m_sweep.a;
    
    if (m_typeA == revoluteJoint)
    {
      RevoluteJoint* revolute = (RevoluteJoint*)def->joint1;
      m_localAnchorC = revolute->m_localAnchorA;
      m_localAnchorA = revolute->m_localAnchorB;
      m_referenceAngleA = revolute->m_referenceAngle;
      m_localAxisC.SetZero();
      
      coordinateA = aA - aC - m_referenceAngleA;
    }
    else
    {
      PrismaticJoint* prismatic = (PrismaticJoint*)def->joint1;
      m_localAnchorC = prismatic->m_localAnchorA;
      m_localAnchorA = prismatic->m_localAnchorB;
      m_referenceAngleA = prismatic->m_referenceAngle;
      m_localAxisC = prismatic->m_localXAxisA;
      
      Vec2 pC = m_localAnchorC;
      Vec2 pA = MulT(xfC.q, Mul(xfA.q, m_localAnchorA) + (xfA.p - xfC.p));
      coordinateA = Dot(pA - pC, m_localAxisC);
    }
    
    m_bodyD = m_joint2->GetBodyA();
    m_bodyB = m_joint2->GetBodyB();
    
    // Body B on joint2 must be dynamic
    IK_ASSERT(m_bodyB->type_ == BodyType::DynamicBody);
    
    // Get geometry of joint2
    Transform xfB = m_bodyB->m_xf;
    float aB = m_bodyB->m_sweep.a;
    Transform xfD = m_bodyD->m_xf;
    float aD = m_bodyD->m_sweep.a;
    
    if (m_typeB == revoluteJoint)
    {
      RevoluteJoint* revolute = (RevoluteJoint*)def->joint2;
      m_localAnchorD = revolute->m_localAnchorA;
      m_localAnchorB = revolute->m_localAnchorB;
      m_referenceAngleB = revolute->m_referenceAngle;
      m_localAxisD.SetZero();
      
      coordinateB = aB - aD - m_referenceAngleB;
    }
    else
    {
      PrismaticJoint* prismatic = (PrismaticJoint*)def->joint2;
      m_localAnchorD = prismatic->m_localAnchorA;
      m_localAnchorB = prismatic->m_localAnchorB;
      m_referenceAngleB = prismatic->m_referenceAngle;
      m_localAxisD = prismatic->m_localXAxisA;
      
      Vec2 pD = m_localAnchorD;
      Vec2 pB = MulT(xfD.q, Mul(xfB.q, m_localAnchorB) + (xfB.p - xfD.p));
      coordinateB = Dot(pB - pD, m_localAxisD);
    }
    
    m_ratio = def->ratio;
    
    m_constant = coordinateA + m_ratio * coordinateB;
    
    m_impulse = 0.0f;
  }
  
  void GearJoint::InitVelocityConstraints(const SolverData& data)
  {
    m_indexA = m_bodyA->m_islandIndex;
    m_indexB = m_bodyB->m_islandIndex;
    m_indexC = m_bodyC->m_islandIndex;
    m_indexD = m_bodyD->m_islandIndex;
    m_lcA = m_bodyA->m_sweep.localCenter;
    m_lcB = m_bodyB->m_sweep.localCenter;
    m_lcC = m_bodyC->m_sweep.localCenter;
    m_lcD = m_bodyD->m_sweep.localCenter;
    m_mA = m_bodyA->m_invMass;
    m_mB = m_bodyB->m_invMass;
    m_mC = m_bodyC->m_invMass;
    m_mD = m_bodyD->m_invMass;
    m_iA = m_bodyA->m_invI;
    m_iB = m_bodyB->m_invI;
    m_iC = m_bodyC->m_invI;
    m_iD = m_bodyD->m_invI;
    
    float aA = data.positions[m_indexA].a;
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    
    float aB = data.positions[m_indexB].a;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    float aC = data.positions[m_indexC].a;
    Vec2 vC = data.velocities[m_indexC].v;
    float wC = data.velocities[m_indexC].w;
    
    float aD = data.positions[m_indexD].a;
    Vec2 vD = data.velocities[m_indexD].v;
    float wD = data.velocities[m_indexD].w;
    
    Rot qA(aA), qB(aB), qC(aC), qD(aD);
    
    m_mass = 0.0f;
    
    if (m_typeA == revoluteJoint)
    {
      m_JvAC.SetZero();
      m_JwA = 1.0f;
      m_JwC = 1.0f;
      m_mass += m_iA + m_iC;
    }
    else
    {
      Vec2 u = Mul(qC, m_localAxisC);
      Vec2 rC = Mul(qC, m_localAnchorC - m_lcC);
      Vec2 rA = Mul(qA, m_localAnchorA - m_lcA);
      m_JvAC = u;
      m_JwC = Cross(rC, u);
      m_JwA = Cross(rA, u);
      m_mass += m_mC + m_mA + m_iC * m_JwC * m_JwC + m_iA * m_JwA * m_JwA;
    }
    
    if (m_typeB == revoluteJoint)
    {
      m_JvBD.SetZero();
      m_JwB = m_ratio;
      m_JwD = m_ratio;
      m_mass += m_ratio * m_ratio * (m_iB + m_iD);
    }
    else
    {
      Vec2 u = Mul(qD, m_localAxisD);
      Vec2 rD = Mul(qD, m_localAnchorD - m_lcD);
      Vec2 rB = Mul(qB, m_localAnchorB - m_lcB);
      m_JvBD = m_ratio * u;
      m_JwD = m_ratio * Cross(rD, u);
      m_JwB = m_ratio * Cross(rB, u);
      m_mass += m_ratio * m_ratio * (m_mD + m_mB) + m_iD * m_JwD * m_JwD + m_iB * m_JwB * m_JwB;
    }
    
    // Compute effective mass.
    m_mass = m_mass > 0.0f ? 1.0f / m_mass : 0.0f;
    
    if (data.step.warmStarting)
    {
      vA += (m_mA * m_impulse) * m_JvAC;
      wA += m_iA * m_impulse * m_JwA;
      vB += (m_mB * m_impulse) * m_JvBD;
      wB += m_iB * m_impulse * m_JwB;
      vC -= (m_mC * m_impulse) * m_JvAC;
      wC -= m_iC * m_impulse * m_JwC;
      vD -= (m_mD * m_impulse) * m_JvBD;
      wD -= m_iD * m_impulse * m_JwD;
    }
    else
    {
      m_impulse = 0.0f;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
    data.velocities[m_indexC].v = vC;
    data.velocities[m_indexC].w = wC;
    data.velocities[m_indexD].v = vD;
    data.velocities[m_indexD].w = wD;
  }
  
  void GearJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    Vec2 vC = data.velocities[m_indexC].v;
    float wC = data.velocities[m_indexC].w;
    Vec2 vD = data.velocities[m_indexD].v;
    float wD = data.velocities[m_indexD].w;
    
    float Cdot = Dot(m_JvAC, vA - vC) + Dot(m_JvBD, vB - vD);
    Cdot += (m_JwA * wA - m_JwC * wC) + (m_JwB * wB - m_JwD * wD);
    
    float impulse = -m_mass * Cdot;
    m_impulse += impulse;
    
    vA += (m_mA * impulse) * m_JvAC;
    wA += m_iA * impulse * m_JwA;
    vB += (m_mB * impulse) * m_JvBD;
    wB += m_iB * impulse * m_JwB;
    vC -= (m_mC * impulse) * m_JvAC;
    wC -= m_iC * impulse * m_JwC;
    vD -= (m_mD * impulse) * m_JvBD;
    wD -= m_iD * impulse * m_JwD;
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
    data.velocities[m_indexC].v = vC;
    data.velocities[m_indexC].w = wC;
    data.velocities[m_indexD].v = vD;
    data.velocities[m_indexD].w = wD;
  }
  
  bool GearJoint::SolvePositionConstraints(const SolverData& data)
  {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    Vec2 cC = data.positions[m_indexC].c;
    float aC = data.positions[m_indexC].a;
    Vec2 cD = data.positions[m_indexD].c;
    float aD = data.positions[m_indexD].a;
    
    Rot qA(aA), qB(aB), qC(aC), qD(aD);
    
    float linearError = 0.0f;
    
    float coordinateA, coordinateB;
    
    Vec2 JvAC, JvBD;
    float JwA, JwB, JwC, JwD;
    float mass = 0.0f;
    
    if (m_typeA == revoluteJoint)
    {
      JvAC.SetZero();
      JwA = 1.0f;
      JwC = 1.0f;
      mass += m_iA + m_iC;
      
      coordinateA = aA - aC - m_referenceAngleA;
    }
    else
    {
      Vec2 u = Mul(qC, m_localAxisC);
      Vec2 rC = Mul(qC, m_localAnchorC - m_lcC);
      Vec2 rA = Mul(qA, m_localAnchorA - m_lcA);
      JvAC = u;
      JwC = Cross(rC, u);
      JwA = Cross(rA, u);
      mass += m_mC + m_mA + m_iC * JwC * JwC + m_iA * JwA * JwA;
      
      Vec2 pC = m_localAnchorC - m_lcC;
      Vec2 pA = MulT(qC, rA + (cA - cC));
      coordinateA = Dot(pA - pC, m_localAxisC);
    }
    
    if (m_typeB == revoluteJoint)
    {
      JvBD.SetZero();
      JwB = m_ratio;
      JwD = m_ratio;
      mass += m_ratio * m_ratio * (m_iB + m_iD);
      
      coordinateB = aB - aD - m_referenceAngleB;
    }
    else
    {
      Vec2 u = Mul(qD, m_localAxisD);
      Vec2 rD = Mul(qD, m_localAnchorD - m_lcD);
      Vec2 rB = Mul(qB, m_localAnchorB - m_lcB);
      JvBD = m_ratio * u;
      JwD = m_ratio * Cross(rD, u);
      JwB = m_ratio * Cross(rB, u);
      mass += m_ratio * m_ratio * (m_mD + m_mB) + m_iD * JwD * JwD + m_iB * JwB * JwB;
      
      Vec2 pD = m_localAnchorD - m_lcD;
      Vec2 pB = MulT(qD, rB + (cB - cD));
      coordinateB = Dot(pB - pD, m_localAxisD);
    }
    
    float C = (coordinateA + m_ratio * coordinateB) - m_constant;
    
    float impulse = 0.0f;
    if (mass > 0.0f)
    {
      impulse = -C / mass;
    }
    
    cA += m_mA * impulse * JvAC;
    aA += m_iA * impulse * JwA;
    cB += m_mB * impulse * JvBD;
    aB += m_iB * impulse * JwB;
    cC -= m_mC * impulse * JvAC;
    aC -= m_iC * impulse * JwC;
    cD -= m_mD * impulse * JvBD;
    aD -= m_iD * impulse * JwD;
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    data.positions[m_indexC].c = cC;
    data.positions[m_indexC].a = aC;
    data.positions[m_indexD].c = cD;
    data.positions[m_indexD].a = aD;
    
    // TODO_ERIN not implemented
    return linearError < LinearSlop;
  }
  
  Vec2 GearJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 GearJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 GearJoint::GetReactionForce(float inv_dt) const
  {
    Vec2 P = m_impulse * m_JvAC;
    return inv_dt * P;
  }
  
  float GearJoint::GetReactionTorque(float inv_dt) const
  {
    float L = m_impulse * m_JwA;
    return inv_dt * L;
  }
  
  void GearJoint::SetRatio(float ratio)
  {
    IK_ASSERT(IsValid(ratio));
    m_ratio = ratio;
  }
  
  float GearJoint::GetRatio() const
  {
    return m_ratio;
  }
    
}
