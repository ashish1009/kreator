//
//  joint.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  class Body;
  class Draw;
  class Joint;
  class BlockAllocator;
  struct SolverData;

  enum JointType {
    unknownJoint,
    revoluteJoint,
    prismaticJoint,
    distanceJoint,
    pulleyJoint,
    mouseJoint,
    gearJoint,
    wheelJoint,
    weldJoint,
    frictionJoint,
    ropeJoint,
    motorJoint
  };
  
  struct Jacobian {
    Vec2 linear;
    float angularA;
    float angularB;
  };

  /// A joint edge is used to connect bodies and joints together
  /// in a joint graph where each body is a node and each joint
  /// is an edge. A joint edge belongs to a doubly linked list
  /// maintained in each attached body. Each joint has two joint
  /// nodes, one for each attached body.
  struct JointEdge {
    Body* other;      ///< provides quick access to the other body attached.
    Joint* joint;      ///< the joint
    JointEdge* prev;    ///< the previous joint edge in the body's joint list
    JointEdge* next;    ///< the next joint edge in the body's joint list
  };

  /// Joint definitions are used to construct joints.
  struct JointDef {
    JointDef() {
      type = unknownJoint;
      bodyA = nullptr;
      bodyB = nullptr;
      collideConnected = false;
    }
    
    /// The joint type is set automatically for concrete joint types.
    JointType type;
    
    /// Use this to attach application specific data to your joints.
    JointUserData userData;
    
    /// The first attached body.
    Body* bodyA;
    
    /// The second attached body.
    Body* bodyB;
    
    /// Set this flag to true if the attached bodies should collide.
    bool collideConnected;
  };
  
  /// Utility to compute linear stiffness values from frequency and damping ratio
  void LinearStiffness(float& stiffness, float& damping,
                       float frequencyHertz, float dampingRatio,
                       const Body* bodyA, const Body* bodyB);
  
  /// Utility to compute rotational stiffness values frequency and damping ratio
  void AngularStiffness(float& stiffness, float& damping,
                        float frequencyHertz, float dampingRatio,
                        const Body* bodyA, const Body* bodyB);
  
  /// The base joint class. Joints are used to constraint two bodies together in
  /// various fashions. Some joints also feature limits and motors.
  class Joint {
  public:
    /// Get the type of the concrete joint.
    JointType GetType() const;
    
    /// Get the first body attached to this joint.
    Body* GetBodyA();
    
    /// Get the second body attached to this joint.
    Body* GetBodyB();
    
    /// Get the anchor point on bodyA in world coordinates.
    virtual Vec2 GetAnchorA() const = 0;
    
    /// Get the anchor point on bodyB in world coordinates.
    virtual Vec2 GetAnchorB() const = 0;
    
    /// Get the reaction force on bodyB at the joint anchor in Newtons.
    virtual Vec2 GetReactionForce(float inv_dt) const = 0;
    
    /// Get the reaction torque on bodyB in N*m.
    virtual float GetReactionTorque(float inv_dt) const = 0;
    
    /// Get the next joint the world joint list.
    Joint* GetNext();
    const Joint* GetNext() const;
    
    /// Get the user data pointer.
    JointUserData& GetUserData();
    
    /// Short-cut function to determine if either body is enabled.
    bool IsEnabled() const;
    
    /// Get collide connected.
    /// Note: modifying the collide connect flag won't work correctly because
    /// the flag is only checked when fixture AABBs begin to overlap.
    bool GetCollideConnected() const;
    
    /// Shift the origin for any points stored in world coordinates.
    virtual void ShiftOrigin(const Vec2& newOrigin) { NOT_USED(newOrigin);  }
    
    /// Debug draw this joint
    virtual void Draw(Draw* draw) const;
    
  protected:
    friend class World;
    friend class Body;
    friend class Island;
    friend class GearJoint;
    
    static Joint* Create(const JointDef* def, BlockAllocator* allocator);
    static void Destroy(Joint* joint, BlockAllocator* allocator);
    
    Joint(const JointDef* def);
    virtual ~Joint() {}
    
    virtual void InitVelocityConstraints(const SolverData& data) = 0;
    virtual void SolveVelocityConstraints(const SolverData& data) = 0;
    
    // This returns true if the position errors are within tolerance.
    virtual bool SolvePositionConstraints(const SolverData& data) = 0;
    
    JointType m_type;
    Joint* m_prev;
    Joint* m_next;
    JointEdge m_edgeA;
    JointEdge m_edgeB;
    Body* m_bodyA;
    Body* m_bodyB;
    
    int32_t m_index;
    
    bool m_islandFlag;
    bool m_collideConnected;
    
    JointUserData m_userData;
  };
  
  inline JointType Joint::GetType() const {
    return m_type;
  }
  
  inline Body* Joint::GetBodyA() {
    return m_bodyA;
  }
  
  inline Body* Joint::GetBodyB() {
    return m_bodyB;
  }
  
  inline Joint* Joint::GetNext() {
    return m_next;
  }
  
  inline const Joint* Joint::GetNext() const {
    return m_next;
  }
  
  inline JointUserData& Joint::GetUserData() {
    return m_userData;
  }
  
  inline bool Joint::GetCollideConnected() const {
    return m_collideConnected;
  }

  
}
