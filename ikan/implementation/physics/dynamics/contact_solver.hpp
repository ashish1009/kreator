//
//  contact_solver.hpp
//  ikan
//
//  Created by Ashish . on 16/01/23.
//

#pragma once

#include "collision.hpp"
#include "physics_math.hpp"
#include "time_setup.hpp"

namespace physics {
  
  class Contact;
  class Body;
  class StackAllocator;
  struct ContactPositionConstraint;
  
  struct VelocityConstraintPoint {
    Vec2 rA;
    Vec2 rB;
    float normalImpulse;
    float tangentImpulse;
    float normalMass;
    float tangentMass;
    float velocityBias;
  };
  
  struct ContactVelocityConstraint {
    VelocityConstraintPoint points[MaxManifoldPoints];
    Vec2 normal;
    Mat22 normalMass;
    Mat22 K;
    int32_t indexA;
    int32_t indexB;
    float invMassA, invMassB;
    float invIA, invIB;
    float friction;
    float restitution;
    float threshold;
    float tangentSpeed;
    int32_t pointCount;
    int32_t contactIndex;
  };
  
  struct ContactSolverDef {
    TimeStep step;
    Contact** contacts;
    int32_t count;
    Position* positions;
    Velocity* velocities;
    StackAllocator* allocator;
  };
  
  class ContactSolver {
  public:
    ContactSolver(ContactSolverDef* def);
    ~ContactSolver();
    
    void InitializeVelocityConstraints();
    
    void WarmStart();
    void SolveVelocityConstraints();
    void StoreImpulses();
    
    bool SolvePositionConstraints();
    bool SolveTOIPositionConstraints(int32_t toiIndexA, int32_t toiIndexB);
    
    TimeStep m_step;
    Position* m_positions;
    Velocity* m_velocities;
    StackAllocator* m_allocator;
    ContactPositionConstraint* m_positionConstraints;
    ContactVelocityConstraint* m_velocityConstraints;
    Contact** m_contacts;
    int m_count;
  };
  
}
