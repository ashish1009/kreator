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
  
  void PrismaticJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis)
  {
    bodyA = bA;
    bodyB = bB;
    localAnchorA = bodyA->GetLocalPoint(anchor);
    localAnchorB = bodyB->GetLocalPoint(anchor);
    localAxisA = bodyA->GetLocalVector(axis);
    referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
  }
  
  PrismaticJoint::PrismaticJoint(const PrismaticJointDef* def)
  : Joint(def)
  {
    m_localAnchorA = def->localAnchorA;
    m_localAnchorB = def->localAnchorB;
    m_localXAxisA = def->localAxisA;
    m_localXAxisA.Normalize();
    m_localYAxisA = Cross(1.0f, m_localXAxisA);
    m_referenceAngle = def->referenceAngle;
    
    m_impulse.SetZero();
    m_axialMass = 0.0f;
    m_motorImpulse = 0.0f;
    m_lowerImpulse = 0.0f;
    m_upperImpulse = 0.0f;
    
    m_lowerTranslation = def->lowerTranslation;
    m_upperTranslation = def->upperTranslation;
    
    IK_ASSERT(m_lowerTranslation <= m_upperTranslation);
    
    m_maxMotorForce = def->maxMotorForce;
    m_motorSpeed = def->motorSpeed;
    m_enableLimit = def->enableLimit;
    m_enableMotor = def->enableMotor;
    
    m_translation = 0.0f;
    m_axis.SetZero();
    m_perp.SetZero();
  }
  
  void PrismaticJoint::InitVelocityConstraints(const SolverData& data)
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
    
    // Compute the effective masses.
    Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
    Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
    Vec2 d = (cB - cA) + rB - rA;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    // Compute motor Jacobian and effective mass.
    {
      m_axis = Mul(qA, m_localXAxisA);
      m_a1 = Cross(d + rA, m_axis);
      m_a2 = Cross(rB, m_axis);
      
      m_axialMass = mA + mB + iA * m_a1 * m_a1 + iB * m_a2 * m_a2;
      if (m_axialMass > 0.0f)
      {
        m_axialMass = 1.0f / m_axialMass;
      }
    }
    
    // Prismatic constraint.
    {
      m_perp = Mul(qA, m_localYAxisA);
      
      m_s1 = Cross(d + rA, m_perp);
      m_s2 = Cross(rB, m_perp);
      
      float k11 = mA + mB + iA * m_s1 * m_s1 + iB * m_s2 * m_s2;
      float k12 = iA * m_s1 + iB * m_s2;
      float k22 = iA + iB;
      if (k22 == 0.0f)
      {
        // For bodies with fixed rotation.
        k22 = 1.0f;
      }
      
      m_K.ex.Set(k11, k12);
      m_K.ey.Set(k12, k22);
    }
    
    if (m_enableLimit)
    {
      m_translation = Dot(m_axis, d);
    }
    else
    {
      m_lowerImpulse = 0.0f;
      m_upperImpulse = 0.0f;
    }
    
    if (m_enableMotor == false)
    {
      m_motorImpulse = 0.0f;
    }
    
    if (data.step.warmStarting)
    {
      // Account for variable time step.
      m_impulse *= data.step.dtRatio;
      m_motorImpulse *= data.step.dtRatio;
      m_lowerImpulse *= data.step.dtRatio;
      m_upperImpulse *= data.step.dtRatio;
      
      float axialImpulse = m_motorImpulse + m_lowerImpulse - m_upperImpulse;
      Vec2 P = m_impulse.x * m_perp + axialImpulse * m_axis;
      float LA = m_impulse.x * m_s1 + m_impulse.y + axialImpulse * m_a1;
      float LB = m_impulse.x * m_s2 + m_impulse.y + axialImpulse * m_a2;
      
      vA -= mA * P;
      wA -= iA * LA;
      
      vB += mB * P;
      wB += iB * LB;
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
  
  void PrismaticJoint::SolveVelocityConstraints(const SolverData& data)
  {
    Vec2 vA = data.velocities[m_indexA].v;
    float wA = data.velocities[m_indexA].w;
    Vec2 vB = data.velocities[m_indexB].v;
    float wB = data.velocities[m_indexB].w;
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    // Solve linear motor constraint
    if (m_enableMotor)
    {
      float Cdot = Dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
      float impulse = m_axialMass * (m_motorSpeed - Cdot);
      float oldImpulse = m_motorImpulse;
      float maxImpulse = data.step.dt * m_maxMotorForce;
      m_motorImpulse = Clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
      impulse = m_motorImpulse - oldImpulse;
      
      Vec2 P = impulse * m_axis;
      float LA = impulse * m_a1;
      float LB = impulse * m_a2;
      
      vA -= mA * P;
      wA -= iA * LA;
      vB += mB * P;
      wB += iB * LB;
    }
    
    if (m_enableLimit)
    {
      // Lower limit
      {
        float C = m_translation - m_lowerTranslation;
        float Cdot = Dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_lowerImpulse;
        m_lowerImpulse = Max(m_lowerImpulse + impulse, 0.0f);
        impulse = m_lowerImpulse - oldImpulse;
        
        Vec2 P = impulse * m_axis;
        float LA = impulse * m_a1;
        float LB = impulse * m_a2;
        
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
        float Cdot = Dot(m_axis, vA - vB) + m_a1 * wA - m_a2 * wB;
        float impulse = -m_axialMass * (Cdot + Max(C, 0.0f) * data.step.inv_dt);
        float oldImpulse = m_upperImpulse;
        m_upperImpulse = Max(m_upperImpulse + impulse, 0.0f);
        impulse = m_upperImpulse - oldImpulse;
        
        Vec2 P = impulse * m_axis;
        float LA = impulse * m_a1;
        float LB = impulse * m_a2;
        
        vA += mA * P;
        wA += iA * LA;
        vB -= mB * P;
        wB -= iB * LB;
      }
    }
    
    // Solve the prismatic constraint in block form.
    {
      Vec2 Cdot;
      Cdot.x = Dot(m_perp, vB - vA) + m_s2 * wB - m_s1 * wA;
      Cdot.y = wB - wA;
      
      Vec2 df = m_K.Solve(-Cdot);
      m_impulse += df;
      
      Vec2 P = df.x * m_perp;
      float LA = df.x * m_s1 + df.y;
      float LB = df.x * m_s2 + df.y;
      
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
  
  // A velocity based solver computes reaction forces(impulses) using the velocity constraint solver.Under this context,
  // the position solver is not there to resolve forces.It is only there to cope with integration error.
  //
  // Therefore, the pseudo impulses in the position solver do not have any physical meaning.Thus it is okay if they suck.
  //
  // We could take the active state from the velocity solver.However, the joint might push past the limit when the velocity
  // solver indicates the limit is inactive.
  bool PrismaticJoint::SolvePositionConstraints(const SolverData& data)
  {
    Vec2 cA = data.positions[m_indexA].c;
    float aA = data.positions[m_indexA].a;
    Vec2 cB = data.positions[m_indexB].c;
    float aB = data.positions[m_indexB].a;
    
    Rot qA(aA), qB(aB);
    
    float mA = m_invMassA, mB = m_invMassB;
    float iA = m_invIA, iB = m_invIB;
    
    // Compute fresh Jacobians
    Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
    Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
    Vec2 d = cB + rB - cA - rA;
    
    Vec2 axis = Mul(qA, m_localXAxisA);
    float a1 = Cross(d + rA, axis);
    float a2 = Cross(rB, axis);
    Vec2 perp = Mul(qA, m_localYAxisA);
    
    float s1 = Cross(d + rA, perp);
    float s2 = Cross(rB, perp);
    
    Vec3 impulse;
    Vec2 C1;
    C1.x = Dot(perp, d);
    C1.y = aB - aA - m_referenceAngle;
    
    float linearError = Abs(C1.x);
    float angularError = Abs(C1.y);
    
    bool active = false;
    float C2 = 0.0f;
    if (m_enableLimit)
    {
      float translation = Dot(axis, d);
      if (Abs(m_upperTranslation - m_lowerTranslation) < 2.0f * LinearSlop)
      {
        C2 = translation;
        linearError = Max(linearError, Abs(translation));
        active = true;
      }
      else if (translation <= m_lowerTranslation)
      {
        C2 = Min(translation - m_lowerTranslation, 0.0f);
        linearError = Max(linearError, m_lowerTranslation - translation);
        active = true;
      }
      else if (translation >= m_upperTranslation)
      {
        C2 = Max(translation - m_upperTranslation, 0.0f);
        linearError = Max(linearError, translation - m_upperTranslation);
        active = true;
      }
    }
    
    if (active)
    {
      float k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
      float k12 = iA * s1 + iB * s2;
      float k13 = iA * s1 * a1 + iB * s2 * a2;
      float k22 = iA + iB;
      if (k22 == 0.0f)
      {
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
    else
    {
      float k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
      float k12 = iA * s1 + iB * s2;
      float k22 = iA + iB;
      if (k22 == 0.0f)
      {
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
    
    data.positions[m_indexA].c = cA;
    data.positions[m_indexA].a = aA;
    data.positions[m_indexB].c = cB;
    data.positions[m_indexB].a = aB;
    
    return linearError <= LinearSlop && angularError <= AngularSlop;
  }
  
  Vec2 PrismaticJoint::GetAnchorA() const
  {
    return m_bodyA->GetWorldPoint(m_localAnchorA);
  }
  
  Vec2 PrismaticJoint::GetAnchorB() const
  {
    return m_bodyB->GetWorldPoint(m_localAnchorB);
  }
  
  Vec2 PrismaticJoint::GetReactionForce(float inv_dt) const
  {
    return inv_dt * (m_impulse.x * m_perp + (m_motorImpulse + m_lowerImpulse - m_upperImpulse) * m_axis);
  }
  
  float PrismaticJoint::GetReactionTorque(float inv_dt) const
  {
    return inv_dt * m_impulse.y;
  }
  
  float PrismaticJoint::GetJointTranslation() const
  {
    Vec2 pA = m_bodyA->GetWorldPoint(m_localAnchorA);
    Vec2 pB = m_bodyB->GetWorldPoint(m_localAnchorB);
    Vec2 d = pB - pA;
    Vec2 axis = m_bodyA->GetWorldVector(m_localXAxisA);
    
    float translation = Dot(d, axis);
    return translation;
  }
  
  float PrismaticJoint::GetJointSpeed() const
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
  
  bool PrismaticJoint::IsLimitEnabled() const
  {
    return m_enableLimit;
  }
  
  void PrismaticJoint::EnableLimit(bool flag)
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
  
  float PrismaticJoint::GetLowerLimit() const
  {
    return m_lowerTranslation;
  }
  
  float PrismaticJoint::GetUpperLimit() const
  {
    return m_upperTranslation;
  }
  
  void PrismaticJoint::SetLimits(float lower, float upper)
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
  
  bool PrismaticJoint::IsMotorEnabled() const
  {
    return m_enableMotor;
  }
  
  void PrismaticJoint::EnableMotor(bool flag)
  {
    if (flag != m_enableMotor)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_enableMotor = flag;
    }
  }
  
  void PrismaticJoint::SetMotorSpeed(float speed)
  {
    if (speed != m_motorSpeed)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_motorSpeed = speed;
    }
  }
  
  void PrismaticJoint::SetMaxMotorForce(float force)
  {
    if (force != m_maxMotorForce)
    {
      m_bodyA->SetAwake(true);
      m_bodyB->SetAwake(true);
      m_maxMotorForce = force;
    }
  }
  
  float PrismaticJoint::GetMotorForce(float inv_dt) const
  {
    return inv_dt * m_motorImpulse;
  }
  
  void PrismaticJoint::Draw(physics::Draw* draw) const
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
