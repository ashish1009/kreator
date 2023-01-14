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
 
  void MotorJointDef::Initialize(Body* bA, Body* bB)
  {
    bodyA = bA;
    bodyB = bB;
    Vec2 xB = bodyB->GetPosition();
    linearOffset = bodyA->GetLocalPoint(xB);
    
    float angleA = bodyA->GetAngle();
    float angleB = bodyB->GetAngle();
    angularOffset = angleB - angleA;
  }
  
  MotorJoint::MotorJoint(const MotorJointDef* def)
  : Joint(def)
  {
    m_linearOffset = def->linearOffset;
    m_angularOffset = def->angularOffset;
    
    m_linearImpulse.SetZero();
    m_angularImpulse = 0.0f;
    
    m_maxForce = def->maxForce;
    m_maxTorque = def->maxTorque;
    m_correctionFactor = def->correctionFactor;
  }
  
  void MotorJoint::InitVelocityConstraints(const SolverData& data)
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
    
    // Compute the effective mass matrix.
    m_rA = Mul(qA, m_linearOffset - m_localCenterA);
    m_rB = Mul(qB, -m_localCenterB);
    
    // J = [-I -r1_skew I r2_skew]
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x,          -r1y*iA-r2y*iB]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB,           r1x*iA+r2x*iB]
    //     [          -r1y*iA-r2y*iB,           r1x*iA+r2x*iB,                   iA+iB]
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    // Upper 2 by 2 of K for point to point
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
    
    m_linearError = cB + m_rB - cA - m_rA;
    m_angularError = aB - aA - m_angularOffset;
    
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
  
  void MotorJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    float h = data.step.dt;
    float inv_h = data.step.inv_dt;
    
    // Solve angular friction
    {
      float Cdot = wB - wA + inv_h * m_correctionFactor * m_angularError;
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
      Vec2 Cdot = vB + Cross(wB, m_rB) - vA - Cross(wA, m_rA) + inv_h * m_correctionFactor * m_linearError;
      
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
  
  bool MotorJoint::SolvePositionConstraints(const SolverData& data)
  {
    NOT_USED(data);
    
    return true;
  }
  
  Vec2 MotorJoint::GetAnchorA() const
  {
    return m_bodyA->GetPosition();
  }
  
  Vec2 MotorJoint::GetAnchorB() const
  {
    return m_bodyB->GetPosition();
  }
  
  Vec2 MotorJoint::GetReactionForce(float inv_dt) const
  {
    return inv_dt * m_linearImpulse;
  }
  
  float MotorJoint::GetReactionTorque(float inv_dt) const
  {
    return inv_dt * m_angularImpulse;
  }
  
  void MotorJoint::SetMaxForce(float force)
  {
    IK_ASSERT(IsValid(force) && force >= 0.0f);
    m_maxForce = force;
  }
  
  float MotorJoint::GetMaxForce() const
  {
    return m_maxForce;
  }
  
  void MotorJoint::SetMaxTorque(float torque)
  {
    IK_ASSERT(IsValid(torque) && torque >= 0.0f);
    m_maxTorque = torque;
  }
  
  float MotorJoint::GetMaxTorque() const
  {
    return m_maxTorque;
  }
  
  void MotorJoint::SetCorrectionFactor(float factor)
  {
    IK_ASSERT(IsValid(factor) && 0.0f <= factor && factor <= 1.0f);
    m_correctionFactor = factor;
  }
  
  float MotorJoint::GetCorrectionFactor() const
  {
    return m_correctionFactor;
  }
  
  void MotorJoint::SetLinearOffset(const Vec2& linearOffset)
  {
    if (linearOffset.x != m_linearOffset.x || linearOffset.y != m_linearOffset.y)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_linearOffset = linearOffset;
    }
  }
  
  const Vec2& MotorJoint::GetLinearOffset() const
  {
    return m_linearOffset;
  }
  
  void MotorJoint::SetAngularOffset(float angularOffset)
  {
    if (angularOffset != m_angularOffset)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_angularOffset = angularOffset;
    }
  }
  
  float MotorJoint::GetAngularOffset() const
  {
    return m_angularOffset;
  }

  
}
