//
//  distance_joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "wheel_joint.hpp"
#include "gear_joint.hpp"
#include "body.hpp"
#include "draw.hpp"
#include "time_setup.hpp"
#include "revolute_joint.hpp"
#include "prismatic_joint.hpp"

namespace physics {
 
  void WheelJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis)
  {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    localAxisA = bodyA->GetLocalVector(axis);
  }
  
  WheelJoint::WheelJoint(const WheelJointDef* def)
  : Joint(def)
  {
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    m_localXAxisA = def->localAxisA;
    m_localYAxisA = Cross(1.0f, m_localXAxisA);
    
    m_mass = 0.0f;
    m_impulse = 0.0f;
    m_motorMass = 0.0f;
    m_motorImpulse = 0.0f;
    m_springMass = 0.0f;
    m_springImpulse = 0.0f;
    
    m_axialMass = 0.0f;
    m_lowerImpulse = 0.0f;
    m_upperImpulse = 0.0f;
    m_lowerTranslation = def->lowerTranslation;
    m_upperTranslation = def->upperTranslation;
    m_enableLimit = def->enableLimit;
    
    m_maxMotorTorque = def->maxMotorTorque;
    m_motorSpeed = def->motorSpeed;
    m_enableMotor = def->enableMotor;
    
    m_bias = 0.0f;
    m_gamma = 0.0f;
    
    m_ax.SetZero();
    m_ay.SetZero();
    
    m_stiffness = def->stiffness;
    m_damping = def->damping;
  }
  
  void WheelJoint::InitVelocityConstraints(const SolverData& data)
  {
    m_indexA = m_bodyA->m_islandIndex;
    m_indexB = m_bodyB->m_islandIndex;
    m_localCenterA = m_bodyA->m_sweep.localCenter;
    m_localCenterB = m_bodyB->m_sweep.localCenter;
    m_invMassA = m_bodyA->m_invMass;
    m_invMassB = m_bodyB->m_invMass;
    m_invIA = m_bodyA->m_invI;
    m_invIB = m_bodyB->m_invI;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    Rot qA(aA), qB(aB);
    
    // Compute the effective masses.
    Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
    Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
    Vec2 d = cB + rB - cA - rA;
    
    // Point to line constraint
    {
      m_ay = Mul(qA, m_localYAxisA);
      m_sAy = Cross(d + rA, m_ay);
      m_sBy = Cross(rB, m_ay);
      
      m_mass = mA + mB + iA * m_sAy * m_sAy + iB * m_sBy * m_sBy;
      
      if (m_mass > 0.0f)
      {
        m_mass = 1.0f / m_mass;
      }
    }
    
    // Spring constraint
    m_ax = Mul(qA, m_localXAxisA);
    m_sAx = Cross(d + rA, m_ax);
    m_sBx = Cross(rB, m_ax);
    
    const float invMass = mA + mB + iA * m_sAx * m_sAx + iB * m_sBx * m_sBx;
    if (invMass > 0.0f)
    {
      m_axialMass = 1.0f / invMass;
    }
    else
    {
      m_axialMass = 0.0f;
    }
    
    m_springMass = 0.0f;
    m_bias = 0.0f;
    m_gamma = 0.0f;
    
    if (m_stiffness > 0.0f && invMass > 0.0f)
    {
      m_springMass = 1.0f / invMass;
      
      float C = Dot(d, m_ax);
      
      // magic formulas
      float h = data.step.dt;
      m_gamma = h * (m_damping + h * m_stiffness);
      if (m_gamma > 0.0f)
      {
        m_gamma = 1.0f / m_gamma;
      }
      
      m_bias = C * h * m_stiffness * m_gamma;
      
      m_springMass = invMass + m_gamma;
      if (m_springMass > 0.0f)
      {
        m_springMass = 1.0f / m_springMass;
      }
    }
    else
    {
      m_springImpulse = 0.0f;
    }
    
    if (m_enableLimit)
    {
      m_translation = Dot(m_ax, d);
    }
    else
    {
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    if (m_enableMotor)
    {
      m_motorMass = iA + iB;
      if (m_motorMass > 0.0f)
      {
        m_motorMass = 1.0f / m_motorMass;
      }
    }
    else
    {
      m_motorMass = 0.0f;
      m_motorImpulse = 0.0f;
    }
    
    if (data.step.warmStarting)
    {
      // Account for variable time step.
      m_impulse *= data.step.dtRatio;
      m_springImpulse *= data.step.dtRatio;
      m_motorImpulse *= data.step.dtRatio;
      
      float axialImpulse = m_springImpulse + m_lowerImpulse - m_upperImpulse;
      Vec2 P = m_impulse * m_ay + axialImpulse * m_ax;
      float LA = m_impulse * m_sAy + axialImpulse * m_sAx + m_motorImpulse;
      float LB = m_impulse * m_sBy + axialImpulse * m_sBx + m_motorImpulse;
      
      vA -= m_invMassA * P;
      wA -= m_invIA * LA;
      
      vB += m_invMassB * P;
      wB += m_invIB * LB;
    }
    else
    {
      m_impulse = 0.0f;
      m_springImpulse = 0.0f;
      m_motorImpulse = 0.0f;
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  void WheelJoint::SolveVelocityConstraints(const SolverData& data)
  {
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    // Solve spring constraint
    {
      float Cdot = Dot(m_ax, vB - vA) + m_sBx * wB - m_sAx * wA;
      float impulse = -m_springMass * (Cdot + m_bias + m_gamma * m_springImpulse);
      m_springImpulse += impulse;
      
      Vec2 P = impulse * m_ax;
      float LA = impulse * m_sAx;
      float LB = impulse * m_sBx;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
    }
    
    // Solve rotational motor constraint
    {
      float Cdot = wB - wA - m_motorSpeed;
      float impulse = -m_motorMass * Cdot;
      
      float oldImpulse = m_motorImpulse;
      float maxImpulse = data.step.dt * m_maxMotorTorque;
      m_motorImpulse = Clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
      impulse = m_motorImpulse - oldImpulse;
      
      wA -= iA * impulse;
      wB += iB * impulse;
    }
    
    if (m_enableLimit)
    {
      // Lower limit
      {
        float C = m_translation - m_lowerTranslation;
        float Cdot = Dot(m_ax, vB - vA) + m_sBx * wB - m_sAx * wA;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_lowerImpulse;
        m_lowerImpulse = Max(m_lowerImpulse + impulse, 0.0f);
        impulse = m_lowerImpulse - oldImpulse;
        
        Vec2 P = impulse * m_ax;
        float LA = impulse * m_sAx;
        float LB = impulse * m_sBx;
        
        vA -= mA * P;
        wA -= iA * LA;
        vB += mB * P;
        wB += iB * LB;
      }
      
      // Upper limit
      // Note: signs are flipped to keep C positive when the constraint is satisfied.
      // This also keeps the impulse positive when the limit is active.
      {
        float C = m_upperTranslation - m_translation;
        float Cdot = Dot(m_ax, vA - vB) + m_sAx * wA - m_sBx * wB;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_upperImpulse;
        m_upperImpulse = Max(m_upperImpulse + impulse, 0.0f);
        impulse = m_upperImpulse - oldImpulse;
        
        Vec2 P = impulse * m_ax;
        float LA = impulse * m_sAx;
        float LB = impulse * m_sBx;
        
        vA += mA * P;
        wA += iA * LA;
        vB -= mB * P;
        wB -= iB * LB;
      }
    }
    
    // Solve point to line constraint
    {
      float Cdot = Dot(m_ay, vB - vA) + m_sBy * wB - m_sAy * wA;
      float impulse = -m_mass * Cdot;
      m_impulse += impulse;
      
      Vec2 P = impulse * m_ay;
      float LA = impulse * m_sAy;
      float LB = impulse * m_sBy;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
    }
    
    data.velocities[m_indexA].v = vA;
    data.velocities[m_indexA].w = wA;
    data.velocities[m_indexB].v = vB;
    data.velocities[m_indexB].w = wB;
  }
  
  bool WheelJoint::SolvePositionConstraints(const SolverData& data)
  {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    
    float linearError = 0.0f;
    
    if (m_enableLimit)
    {
      Rot qA(aA), qB(aB);
      
      Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
      Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
      Vec2 d = (cB - cA) + rB - rA;
      
      Vec2 ax = Mul(qA, m_localXAxisA);
      float sAx = Cross(d + rA, m_ax);
      float sBx = Cross(rB, m_ax);
      
      float C = 0.0f;
      float translation = Dot(ax, d);
      if (Abs(m_upperTranslation - m_lowerTranslation) < 2.0f * LinearSlop)
      {
        C = translation;
      }
      else if (translation <= m_lowerTranslation)
      {
        C = Min(translation - m_lowerTranslation, 0.0f);
      }
      else if (translation >= m_upperTranslation)
      {
        C = Max(translation - m_upperTranslation, 0.0f);
      }
      
      if (C != 0.0f)
      {
        
        float invMass = m_invMassA + m_invMassB + m_invIA * sAx * sAx + m_invIB * sBx * sBx;
        float impulse = 0.0f;
        if (invMass != 0.0f)
        {
          impulse = -C / invMass;
        }
        
        Vec2 P = impulse * ax;
        float LA = impulse * sAx;
        float LB = impulse * sBx;
        
        cA -= m_invMassA * P;
        aA -= m_invIA * LA;
        cB += m_invMassB * P;
        aB += m_invIB * LB;
        
        linearError = Abs(C);
      }
    }
    
    // Solve perpendicular constraint
    {
      Rot qA(aA), qB(aB);
      
      Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
      Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
      Vec2 d = (cB - cA) + rB - rA;
      
      Vec2 ay = Mul(qA, m_localYAxisA);
      
      float sAy = Cross(d + rA, ay);
      float sBy = Cross(rB, ay);
      
      float C = Dot(d, ay);
      
      float invMass = m_invMassA + m_invMassB + m_invIA * m_sAy * m_sAy + m_invIB * m_sBy * m_sBy;
      
      float impulse = 0.0f;
      if (invMass != 0.0f)
      {
        impulse = - C / invMass;
      }
      
      Vec2 P = impulse * ay;
      float LA = impulse * sAy;
      float LB = impulse * sBy;
      
      cA -= m_invMassA * P;
      aA -= m_invIA * LA;
      cB += m_invMassB * P;
      aB += m_invIB * LB;
      
      linearError = Max(linearError, Abs(C));
    }
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    
    return linearError <= LinearSlop;
  }
  
  Vec2 WheelJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 WheelJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 WheelJoint::GetReactionForce(float inv_dt) const
  {
    return inv_dt * (m_impulse * m_ay + (m_springImpulse + m_lowerImpulse - m_upperImpulse) * m_ax);
  }
  
  float WheelJoint::GetReactionTorque(float inv_dt) const
  {
    return inv_dt * m_motorImpulse;
  }
  
  float WheelJoint::GetJointTranslation() const
  {
    Body* bA = m_bodyA;
    Body* bB = m_bodyB;
    
    Vec2 pA = bA->GetWorldPoint(m_localAnchorA);
    Vec2 pB = bB->GetWorldPoint(m_localAnchorB);
    Vec2 d = pB - pA;
    Vec2 axis = bA->GetWorldVector(m_localXAxisA);
    
    float translation = Dot(d, axis);
    return translation;
  }
  
  float WheelJoint::GetJointLinearSpeed() const
  {
    Body* bA = m_bodyA;
    Body* bB = m_bodyB;
    
    Vec2 rA = Mul(bA->m_xf.q, m_localAnchorA - bA->m_sweep.localCenter);
    Vec2 rB = Mul(bB->m_xf.q, m_localAnchorB - bB->m_sweep.localCenter);
    Vec2 p1 = bA->m_sweep.c + rA;
    Vec2 p2 = bB->m_sweep.c + rB;
    Vec2 d = p2 - p1;
    Vec2 axis = Mul(bA->m_xf.q, m_localXAxisA);
    
    Vec2 vA = bA->m_linearVelocity;
    Vec2 vB = bB->m_linearVelocity;
    float wA = bA->m_angularVelocity;
    float wB = bB->m_angularVelocity;
    
    float speed = Dot(d, Cross(wA, axis)) + Dot(axis, vB + Cross(wB, rB) - vA - Cross(wA, rA));
    return speed;
  }
  
  float WheelJoint::GetJointAngle() const
  {
    Body* bA = m_bodyA;
    Body* bB = m_bodyB;
    return bB->m_sweep.a - bA->m_sweep.a;
  }
  
  float WheelJoint::GetJointAngularSpeed() const
  {
    float wA = m_bodyA->m_angularVelocity;
    float wB = m_bodyB->m_angularVelocity;
    return wB - wA;
  }
  
  bool WheelJoint::IsLimitEnabled() const
  {
    return m_enableLimit;
  }
  
  void WheelJoint::EnableLimit(bool flag)
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
  
  float WheelJoint::GetLowerLimit() const
  {
    return m_lowerTranslation;
  }
  
  float WheelJoint::GetUpperLimit() const
  {
    return m_upperTranslation;
  }
  
  void WheelJoint::SetLimits(float lower, float upper)
  {
    IK_ASSERT(lower <= upper);
    if (lower != m_lowerTranslation || upper != m_upperTranslation)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_lowerTranslation = lower;
      m_upperTranslation = upper;
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
  }
  
  bool WheelJoint::IsMotorEnabled() const
  {
    return m_enableMotor;
  }
  
  void WheelJoint::EnableMotor(bool flag)
  {
    if (flag != m_enableMotor)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_enableMotor = flag;
    }
  }
  
  void WheelJoint::SetMotorSpeed(float speed)
  {
    if (speed != m_motorSpeed)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_motorSpeed = speed;
    }
  }
  
  void WheelJoint::SetMaxMotorTorque(float torque)
  {
    if (torque != m_maxMotorTorque)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_maxMotorTorque = torque;
    }
  }
  
  float WheelJoint::GetMotorTorque(float inv_dt) const
  {
    return inv_dt * m_motorImpulse;
  }
  
  void WheelJoint::SetStiffness(float stiffness)
  {
    m_stiffness = stiffness;
  }
  
  float WheelJoint::GetStiffness() const
  {
    return m_stiffness;
  }
  
  void WheelJoint::SetDamping(float damping)
  {
    m_damping = damping;
  }
  
  float WheelJoint::GetDamping() const
  {
    return m_damping;
  }
  
  ///
  void WheelJoint::Draw(physics::Draw* draw) const
  {
    const Transform& xfA = m_bodyA->GetTransform();
    const Transform& xfB = m_bodyB->GetTransform();
    Vec2 pA = Mul(xfA, m_localAnchorA);
    Vec2 pB = Mul(xfB, m_localAnchorB);
    
    Vec2 axis = Mul(xfA.q, m_localXAxisA);
    
    Color c1(0.7f, 0.7f, 0.7f);
    Color c2(0.3f, 0.9f, 0.3f);
    Color c3(0.9f, 0.3f, 0.3f);
    Color c4(0.3f, 0.3f, 0.9f);
    Color c5(0.4f, 0.4f, 0.4f);
    
    draw->DrawSegment(pA, pB, c5);
    
    if (m_enableLimit)
    {
      Vec2 lower = pA + m_lowerTranslation * axis;
      Vec2 upper = pA + m_upperTranslation * axis;
      Vec2 perp = Mul(xfA.q, m_localYAxisA);
      draw->DrawSegment(lower, upper, c1);
      draw->DrawSegment(lower - 0.5f * perp, lower + 0.5f * perp, c2);
      draw->DrawSegment(upper - 0.5f * perp, upper + 0.5f * perp, c3);
    }
    else
    {
      draw->DrawSegment(pA - 1.0f * axis, pA + 1.0f * axis, c1);
    }
    
    draw->DrawPoint(pA, 5.0f, c1);
    draw->DrawPoint(pB, 5.0f, c4);
  }

  
}
