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
 
  void FrictionJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor)
  {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
  }
  
  FrictionJoint::FrictionJoint(const FrictionJointDef* def)
  : Joint(def)
  {
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    
    m_linearImpulse.SetZero();
    m_angularImpulse = 0.0f;
    
    m_maxForce = def->maxForce;
    m_maxTorque = def->maxTorque;
  }
  
  void FrictionJoint::InitVelocityConstraints(const SolverData& data)
  {
    m_indexA = m_bodyA->m_islandIndex;
    m_indexB = m_bodyB->m_islandIndex;
    m_localCenterA = m_bodyA->m_sweep.localCenter;
    m_localCenterB = m_bodyB->m_sweep.localCenter;
    m_invMassA = m_bodyA->m_invMass;
    m_invMassB = m_bodyB->m_invMass;
    m_invIA = m_bodyA->m_invI;
    m_invIB = m_bodyB->m_invI;
    
    float aA = data.positions[m_indexA].a;
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    
    float aB = data.positions[m_indexB].a;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    Rot qA(aA), qB(aB);
    
    // Compute the effective mass matrix.
    m_rA = Mul(qA, m_localAnchorA - m_localCenterA);
    m_rB = Mul(qB, m_localAnchorB - m_localCenterB);
    
    // J = [-I -r1_skew I r2_skew]
    //     [ 0       -1 0       1]
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
    //     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    Mat22 K;
    K.ex.x = mA + mB + iA * m_rA.y * m_rA.y + iB * m_rB.y * m_rB.y;
    K.ex.y = -iA * m_rA.x * m_rA.y - iB * m_rB.x * m_rB.y;
    K.ey.x = K.ex.y;
    K.ey.y = mA + mB + iA * m_rA.x * m_rA.x + iB * m_rB.x * m_rB.x;
    
    m_linearMass = K.GetInverse();
    
    m_angularMass = iA + iB;
    if (m_angularMass > 0.0f)
    {
      m_angularMass = 1.0f / m_angularMass;
    }
    
    if (data.step.warmStarting)
    {
      // Scale impulses to support a variable time step.
      m_linearImpulse *= data.step.dtRatio;
      m_angularImpulse *= data.step.dtRatio;
      
      Vec2 P(m_linearImpulse.x, m_linearImpulse.y);
      vA -= mA * P;
      wA -= iA * (Cross(m_rA, P) + m_angularImpulse);
      vB += mB * P;
      wB += iB * (Cross(m_rB, P) + m_angularImpulse);
    }
    else
    {
      m_linearImpulse.SetZero();
      m_angularImpulse = 0.0f;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  void FrictionJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    float h = data.step.dt;
    
    // Solve angular friction
    {
      float Cdot = wB - wA;
      float impulse = -m_angularMass * Cdot;
      
      float oldImpulse = m_angularImpulse;
      float maxImpulse = h * m_maxTorque;
      m_angularImpulse = Clamp(m_angularImpulse + impulse, -maxImpulse, maxImpulse);
      impulse = m_angularImpulse - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    // Solve linear friction
    {
      Vec2 Cdot = vB + Cross(wB, m_rB) - vA - Cross(wA, m_rA);
      
      Vec2 impulse = -Mul(m_linearMass, Cdot);
      Vec2 oldImpulse = m_linearImpulse;
      m_linearImpulse += impulse;
      
      float maxImpulse = h * m_maxForce;
      
      if (m_linearImpulse.LengthSquared() > maxImpulse * maxImpulse)
      {
        m_linearImpulse.Normalize();
        m_linearImpulse *= maxImpulse;
      }
      
      impulse = m_linearImpulse - oldImpulse;
      
      vA -= mA * impulse;
      wA -= iA * Cross(m_rA, impulse);
      
      vB += mB * impulse;
      wB += iB * Cross(m_rB, impulse);
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  bool FrictionJoint::SolvePositionConstraints(const SolverData& data)
  {
    NOT_USED(data);
    
    return true;
  }
  
  Vec2 FrictionJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 FrictionJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 FrictionJoint::GetReactionForce(float inv_dt) const
  {
    return inv_dt * m_linearImpulse;
  }
  
  float FrictionJoint::GetReactionTorque(float inv_dt) const
  {
    return inv_dt * m_angularImpulse;
  }
  
  void FrictionJoint::SetMaxForce(float force)
  {
    IK_ASSERT(IsValid(force) && force >= 0.0f);
    m_maxForce = force;
  }
  
  float FrictionJoint::GetMaxForce() const
  {
    return m_maxForce;
  }
  
  void FrictionJoint::SetMaxTorque(float torque)
  {
    IK_ASSERT(IsValid(torque) && torque >= 0.0f);
    m_maxTorque = torque;
  }
  
  float FrictionJoint::GetMaxTorque() const
  {
    return m_maxTorque;
  }

  
}
