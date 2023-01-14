//
//  joint.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "joint.hpp"
#include "body.hpp"
#include "block_allocator.hpp"
#include "distance_joint.hpp"
#include "mouse_joint.hpp"
#include "pully_joint.hpp"
#include "gear_joint.hpp"
#include "weld_joint.hpp"
#include "wheel_joint.hpp"
#include "motor_joint.hpp"
#include "prismatic_joint.hpp"
#include "revolute_joint.hpp"
#include "friction_joint.hpp"

namespace physics {
  
  void LinearStiffness(float& stiffness, float& damping,
                       float frequencyHertz, float dampingRatio,
                       const Body* bodyA, const Body* bodyB) {
    float massA = bodyA->GetMass();
    float massB = bodyB->GetMass();
    float mass;
    if (massA > 0.0f && massB > 0.0f) {
      mass = massA * massB / (massA + massB);
    }
    else if (massA > 0.0f) {
      mass = massA;
    }
    else {
      mass = massB;
    }
    
    float omega = 2.0f * PI * frequencyHertz;
    stiffness = mass * omega * omega;
    damping = 2.0f * mass * dampingRatio * omega;
  }
  
  void AngularStiffness(float& stiffness, float& damping,
                        float frequencyHertz, float dampingRatio,
                        const Body* bodyA, const Body* bodyB) {
    float IA = bodyA->GetInertia();
    float IB = bodyB->GetInertia();
    float I;
    if (IA > 0.0f && IB > 0.0f) {
      I = IA * IB / (IA + IB);
    }
    else if (IA > 0.0f) {
      I = IA;
    }
    else {
      I = IB;
    }
    
    float omega = 2.0f * PI * frequencyHertz;
    stiffness = I * omega * omega;
    damping = 2.0f * I * dampingRatio * omega;
  }
  
  Joint* Joint::Create(const JointDef* def, BlockAllocator* allocator) {
    Joint* joint = nullptr;
    
    switch (def->type) {
      case  distanceJoint: {
        void* mem = allocator->Allocate(sizeof(DistanceJoint));
        joint = new (mem) DistanceJoint(static_cast<const DistanceJointDef*>(def));
      }
        break;
        
      case mouseJoint: {
        void* mem = allocator->Allocate(sizeof(MouseJoint));
        joint = new (mem) MouseJoint(static_cast<const MouseJointDef*>(def));
      }
        break;
        
      case prismaticJoint: {
        void* mem = allocator->Allocate(sizeof(PrismaticJoint));
        joint = new (mem) PrismaticJoint(static_cast<const PrismaticJointDef*>(def));
      }
        break;
        
      case revoluteJoint: {
        void* mem = allocator->Allocate(sizeof(RevoluteJoint));
        joint = new (mem) RevoluteJoint(static_cast<const RevoluteJointDef*>(def));
      }
        break;
        
      case pulleyJoint: {
        void* mem = allocator->Allocate(sizeof(PulleyJoint));
        joint = new (mem) PulleyJoint(static_cast<const PulleyJointDef*>(def));
      }
        break;
        
      case gearJoint: {
        void* mem = allocator->Allocate(sizeof(GearJoint));
        joint = new (mem) GearJoint(static_cast<const GearJointDef*>(def));
      }
        break;
        
      case wheelJoint: {
        void* mem = allocator->Allocate(sizeof(WheelJoint));
        joint = new (mem) WheelJoint(static_cast<const WheelJointDef*>(def));
      }
        break;
        
      case weldJoint: {
        void* mem = allocator->Allocate(sizeof(WeldJoint));
        joint = new (mem) WeldJoint(static_cast<const WeldJointDef*>(def));
      }
        break;
        
      case frictionJoint: {
        void* mem = allocator->Allocate(sizeof(FrictionJoint));
        joint = new (mem) FrictionJoint(static_cast<const FrictionJointDef*>(def));
      }
        break;
        
      case motorJoint: {
        void* mem = allocator->Allocate(sizeof(MotorJoint));
        joint = new (mem) MotorJoint(static_cast<const MotorJointDef*>(def));
      }
        break;
        
      default:
        Assert(false);
        break;
    }
    
    return joint;
  }
  
  void Joint::Destroy(Joint* joint, BlockAllocator* allocator) {
    joint->~Joint();
    switch (joint->m_type) {
      case distanceJoint:
        allocator->Free(joint, sizeof(DistanceJoint));
        break;
        
      case mouseJoint:
        allocator->Free(joint, sizeof(MouseJoint));
        break;
        
      case prismaticJoint:
        allocator->Free(joint, sizeof(PrismaticJoint));
        break;
        
      case revoluteJoint:
        allocator->Free(joint, sizeof(RevoluteJoint));
        break;
        
      case pulleyJoint:
        allocator->Free(joint, sizeof(PulleyJoint));
        break;
        
      case gearJoint:
        allocator->Free(joint, sizeof(GearJoint));
        break;
        
      case wheelJoint:
        allocator->Free(joint, sizeof(WheelJoint));
        break;
        
      case weldJoint:
        allocator->Free(joint, sizeof(WeldJoint));
        break;
        
      case frictionJoint:
        allocator->Free(joint, sizeof(FrictionJoint));
        break;
        
      case motorJoint:
        allocator->Free(joint, sizeof(MotorJoint));
        break;
        
      default:
        Assert(false);
        break;
    }
  }
  
  Joint::Joint(const JointDef* def) {
    Assert(def->bodyA != def->bodyB);
    
    m_type = def->type;
    m_prev = nullptr;
    m_next = nullptr;
    m_bodyA = def->bodyA;
    m_bodyB = def->bodyB;
    m_index = 0;
    m_collideConnected = def->collideConnected;
    m_islandFlag = false;
    m_userData = def->userData;
    
    m_edgeA.joint = nullptr;
    m_edgeA.other = nullptr;
    m_edgeA.prev = nullptr;
    m_edgeA.next = nullptr;
    
    m_edgeB.joint = nullptr;
    m_edgeB.other = nullptr;
    m_edgeB.prev = nullptr;
    m_edgeB.next = nullptr;
  }
  
  bool Joint::IsEnabled() const {
    return m_bodyA->IsEnabled() && m_bodyB->IsEnabled();
  }
  
  void Joint::Draw(Draw* draw) const {
    const Transform& xf1 = m_bodyA->GetTransform();
    const Transform& xf2 = m_bodyB->GetTransform();
    Vec2 x1 = xf1.p;
    Vec2 x2 = xf2.p;
    Vec2 p1 = GetAnchorA();
    Vec2 p2 = GetAnchorB();
    
    Color color(0.5f, 0.8f, 0.8f);
    
    switch (m_type) {
      case distanceJoint:
        draw->DrawSegment(p1, p2, color);
        break;
        
      case pulleyJoint: {
        PulleyJoint* pulley = (PulleyJoint*)this;
        Vec2 s1 = pulley->GetGroundAnchorA();
        Vec2 s2 = pulley->GetGroundAnchorB();
        draw->DrawSegment(s1, p1, color);
        draw->DrawSegment(s2, p2, color);
        draw->DrawSegment(s1, s2, color);
      }
        break;
        
      case mouseJoint: {
        Color c;
        c.Set(0.0f, 1.0f, 0.0f);
        draw->DrawPoint(p1, 4.0f, c);
        draw->DrawPoint(p2, 4.0f, c);
        
        c.Set(0.8f, 0.8f, 0.8f);
        draw->DrawSegment(p1, p2, c);
        
      }
        break;
        
      default:
        draw->DrawSegment(x1, p1, color);
        draw->DrawSegment(p1, p2, color);
        draw->DrawSegment(x2, p2, color);
    }
  }

}
