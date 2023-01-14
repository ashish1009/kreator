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
  
  void RevoluteJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor)
  {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
  }
  
  RevoluteJoint::RevoluteJoint(const RevoluteJointDef* def)
  : Joint(def)
  {
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    m_referenceAngle = def->referenceAngle;
    
    m_impulse.SetZero();
    m_axialMass = 0.0f;
    m_motorImpulse = 0.0f;
    m_lowerImpulse = 0.0f;
    m_upperImpulse = 0.0f;
    
    m_lowerAngle = def->lowerAngle;
    m_upperAngle = def->upperAngle;
    m_maxMotorTorque = def->maxMotorTorque;
    m_motorSpeed = def->motorSpeed;
    m_enableLimit = def->enableLimit;
    m_enableMotor = def->enableMotor;
    
    m_angle = 0.0f;
  }
  
  void RevoluteJoint::InitVelocityConstraints(const SolverData& data)
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
    
    m_rA = Mul(qA, m_localAnchorA - m_localCenterA);
    m_rB = Mul(qB, m_localAnchorB - m_localCenterB);
    
    // J = [-I -r1_skew I r2_skew]
    // r_skew = [-ry; rx]
    
    // Matlab
    // K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x]
    //     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB]
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    m_K.ex.x = mA + mB + m_rA.y * m_rA.y * iA + m_rB.y * m_rB.y * iB;
    m_K.ey.x = -m_rA.y * m_rA.x * iA - m_rB.y * m_rB.x * iB;
    m_K.ex.y = m_K.ey.x;
    m_K.ey.y = mA + mB + m_rA.x * m_rA.x * iA + m_rB.x * m_rB.x * iB;
    
    m_axialMass = iA + iB;
    bool fixedRotation;
    if (m_axialMass > 0.0f)
    {
      m_axialMass = 1.0f / m_axialMass;
      fixedRotation = false;
    }
    else
    {
      fixedRotation = true;
    }
    
    m_angle = aB - aA - m_referenceAngle;
    if (m_enableLimit == false || fixedRotation)
    {
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    if (m_enableMotor == false || fixedRotation)
    {
      m_motorImpulse = 0.0f;
    }
    
    if (data.step.warmStarting)
    {
      // Scale impulses to support a variable time step.
      m_impulse *= data.step.dtRatio;
      m_motorImpulse *= data.step.dtRatio;
      m_lowerImpulse *= data.step.dtRatio;
      m_upperImpulse *= data.step.dtRatio;
      
      float axialImpulse = m_motorImpulse + m_lowerImpulse - m_upperImpulse;
      Vec2 P(m_impulse.x, m_impulse.y);
      
      vA -= mA * P;
      wA -= iA * (Cross(m_rA, P) + axialImpulse);
      
      vB += mB * P;
      wB += iB * (Cross(m_rB, P) + axialImpulse);
    }
    else
    {
      m_impulse.SetZero();
      m_motorImpulse = 0.0f;
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  void RevoluteJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    bool fixedRotation = (iA + iB == 0.0f);
    
    // Solve motor constraint.
    if (m_enableMotor && fixedRotation == false)
    {
      float Cdot = wB - wA - m_motorSpeed;
      float impulse = -m_axialMass * Cdot;
      float oldImpulse = m_motorImpulse;
      float maxImpulse = data.step.dt * m_maxMotorTorque;
      m_motorImpulse = Clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
      impulse = m_motorImpulse - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    if (m_enableLimit && fixedRotation == false)
    {
      // Lower limit
      {
        float C = m_angle - m_lowerAngle;
        float Cdot = wB - wA;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_lowerImpulse;
        m_lowerImpulse = Max(m_lowerImpulse + impulse, 0.0f);
        impulse = m_lowerImpulse - oldImpulse;
        
        wA -= iA * impulse;
        wB += iB * impulse;
      }
      
      // Upper limit
      // Note: signs are flipped to keep C positive when the constraint is satisfied.
      // This also keeps the impulse positive when the limit is active.
      {
        float C = m_upperAngle - m_angle;
        float Cdot = wA - wB;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_upperImpulse;
        m_upperImpulse = Max(m_upperImpulse + impulse, 0.0f);
        impulse = m_upperImpulse - oldImpulse;
        
        wA += iA * impulse;
        wB -= iB * impulse;
      }
    }
    
    // Solve point-to-point constraint
    {
      Vec2 Cdot = vB + Cross(wB, m_rB) - vA - Cross(wA, m_rA);
      Vec2 impulse = m_K.Solve(-Cdot);
      
      m_impulse.x += impulse.x;
      m_impulse.y += impulse.y;
      
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
  
  bool RevoluteJoint::SolvePositionConstraints(const SolverData& data)
  {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    
    Rot qA(aA), qB(aB);
    
    float angularError = 0.0f;
    float positionError = 0.0f;
    
    bool fixedRotation = (m_invIA + m_invIB == 0.0f);
    
    // Solve angular limit constraint
    if (m_enableLimit && fixedRotation == false)
    {
      float angle = aB - aA - m_referenceAngle;
      float C = 0.0f;
      
      if (Abs(m_upperAngle - m_lowerAngle) < 2.0f * AngularSlop)
      {
        // Prevent large angular corrections
        C = Clamp(angle - m_lowerAngle, -MaxAngularCorrection, MaxAngularCorrection);
      }
      else if (angle <= m_lowerAngle)
      {
        // Prevent large angular corrections and allow some slop.
        C = Clamp(angle - m_lowerAngle + AngularSlop, - MaxAngularCorrection, 0.0f);
      }
      else if (angle >= m_upperAngle)
      {
        // Prevent large angular corrections and allow some slop.
        C = Clamp(angle - m_upperAngle - AngularSlop, 0.0f, MaxAngularCorrection);
      }
      
      float limitImpulse = -m_axialMass * C;
      aA -= m_invIA * limitImpulse;
      aB += m_invIB * limitImpulse;
      angularError = Abs(C);
    }
    
    // Solve point-to-point constraint.
    {
      qA.Set(aA);
      qB.Set(aB);
      Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
      Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
      
      Vec2 C = cB + rB - cA - rA;
      positionError = C.Length();
      
      float mA = m_invMassA, mB = m_invMassB;
      float iA = m_invIA, iB = m_invIB;
      
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
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    
    return positionError <= LinearSlop && angularError <= AngularSlop;
  }
  
  Vec2 RevoluteJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 RevoluteJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 RevoluteJoint::GetReactionForce(float inv_dt) const
  {
    Vec2 P(m_impulse.x, m_impulse.y);
    return inv_dt * P;
  }
  
  float RevoluteJoint::GetReactionTorque(float inv_dt) const
  {
    return inv_dt * (m_motorImpulse + m_lowerImpulse - m_upperImpulse);
  }
  
  float RevoluteJoint::GetJointAngle() const
  {
    Body* bA = m_bodyA;
    Body* bB = m_bodyB;
    return bB->m_sweep.a - bA->m_sweep.a - m_referenceAngle;
  }
  
  float RevoluteJoint::GetJointSpeed() const
  {
    Body* bA = m_bodyA;
    Body* bB = m_bodyB;
    return bB->m_angularVelocity - bA->m_angularVelocity;
  }
  
  bool RevoluteJoint::IsMotorEnabled() const
  {
    return m_enableMotor;
  }
  
  void RevoluteJoint::EnableMotor(bool flag)
  {
    if (flag != m_enableMotor)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_enableMotor = flag;
    }
  }
  
  float RevoluteJoint::GetMotorTorque(float inv_dt) const
  {
    return inv_dt * m_motorImpulse;
  }
  
  void RevoluteJoint::SetMotorSpeed(float speed)
  {
    if (speed != m_motorSpeed)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_motorSpeed = speed;
    }
  }
  
  void RevoluteJoint::SetMaxMotorTorque(float torque)
  {
    if (torque != m_maxMotorTorque)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_maxMotorTorque = torque;
    }
  }
  
  bool RevoluteJoint::IsLimitEnabled() const
  {
    return m_enableLimit;
  }
  
  void RevoluteJoint::EnableLimit(bool flag)
  {
    if (flag != m_enableLimit)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_enableLimit = flag;
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
  }
  
  float RevoluteJoint::GetLowerLimit() const
  {
    return m_lowerAngle;
  }
  
  float RevoluteJoint::GetUpperLimit() const
  {
    return m_upperAngle;
  }
  
  void RevoluteJoint::SetLimits(float lower, float upper)
  {
    IK_ASSERT(lower <= upper);
    
    if (lower != m_lowerAngle || upper != m_upperAngle)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
      m_lowerAngle = lower;
      m_upperAngle = upper;
    }
  }
  
  void RevoluteJoint::Draw(physics::Draw* draw) const
  {
    const Transform& xfA = m_bodyA->GetTransform();
    const Transform& xfB = m_bodyB->GetTransform();
    Vec2 pA = Mul(xfA, m_localAnchorA);
    Vec2 pB = Mul(xfB, m_localAnchorB);
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.3f, 0.3f, 0.9f);
    Color c5(0.4f, 0.4f, 0.4f);
    
    draw->DrawPoint(pA, 5.0f, c4);
    draw->DrawPoint(pB, 5.0f, c5);
    
    float aA = m_bodyA->GetAngle();
    float aB = m_bodyB->GetAngle();
    float angle = aB - aA - m_referenceAngle;
    
    const float L = 0.5f;
    
    Vec2 r = L * Vec2(cosf(angle), sinf(angle));
    draw->DrawSegment(pB, pB + r, c1);
    draw->DrawCircle(pB, L, c1);
    
    if (m_enableLimit)
    {
      Vec2 rlo = L * Vec2(cosf(m_lowerAngle), sinf(m_lowerAngle));
      Vec2 rhi = L * Vec2(cosf(m_upperAngle), sinf(m_upperAngle));
      
      draw->DrawSegment(pB, pB + rlo, c2);
      draw->DrawSegment(pB, pB + rhi, c3);
    }
    
    Color color(0.5f, 0.8f, 0.8f);
    draw->DrawSegment(xfA.p, pA, color);
    draw->DrawSegment(pA, pB, color);
    draw->DrawSegment(xfB.p, pB, color);
  }

}
