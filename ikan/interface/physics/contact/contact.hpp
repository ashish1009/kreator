//
//  contact.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "collision.hpp"
#include "fixture.hpp"
#include "shape.hpp"

namespace physics {
  
  class Body;
  class Contact;
  class BlockAllocator;
  class ContactListener;
  
  /// Friction mixing law. The idea is to allow either fixture to drive the friction to zero.
  /// For example, anything slides on ice.
  inline float MixFriction(float friction1, float friction2) {
    return sqrt(friction1 * friction2);
  }
  
  /// Restitution mixing law. The idea is allow for anything to bounce off an inelastic surface.
  /// For example, a superball bounces on anything.
  inline float MixRestitution(float restitution1, float restitution2) {
    return restitution1 > restitution2 ? restitution1 : restitution2;
  }

  /// Restitution mixing law. This picks the lowest value.
  inline float MixRestitutionThreshold(float threshold1, float threshold2) {
    return threshold1 < threshold2 ? threshold1 : threshold2;
  }

  typedef Contact* ContactCreateFcn(Fixture* fixtureA, int32_t indexA,
                                    Fixture* fixtureB, int32_t indexB,
                                    BlockAllocator* allocator);
  typedef void ContactDestroyFcn(Contact* contact, BlockAllocator* allocator);

  struct ContactRegister {
    ContactCreateFcn* createFcn;
    ContactDestroyFcn* destroyFcn;
    bool primary;
  };

  /// A contact edge is used to connect bodies and contacts together
  /// in a contact graph where each body is a node and each contact
  /// is an edge. A contact edge belongs to a doubly linked list
  /// maintained in each attached body. Each contact has two contact
  /// nodes, one for each attached body.
  struct ContactEdge {
    Body* other;      ///< provides quick access to the other body attached.
    Contact* contact;    ///< the contact
    ContactEdge* prev;  ///< the previous contact edge in the body's contact list
    ContactEdge* next;  ///< the next contact edge in the body's contact list
  };
  
  /// The class manages contact between two shapes. A contact exists for each overlapping
  /// AABB in the broad-phase (except if filtered). Therefore a contact object may exist
  /// that has no contact points.
  class Contact {
  public:
    
    /// Get the contact manifold. Do not modify the manifold unless you understand the
    /// internals of Box2D.
    Manifold* GetManifold();
    const Manifold* GetManifold() const;
    
    /// Get the world manifold.
    void GetWorldManifold(WorldManifold* worldManifold) const;
    
    /// Is this contact touching?
    bool IsTouching() const;

    /// Enable/disable this contact. This can be used inside the pre-solve
    /// contact listener. The contact is only disabled for the current
    /// time step (or sub-step in continuous collisions).
    void SetEnabled(bool flag);

    /// Has this contact been disabled?
    bool IsEnabled() const;

    /// Get the next contact in the world's contact list.
    Contact* GetNext();
    const Contact* GetNext() const;

    /// Get fixture A in this contact.
    Fixture* GetFixtureA();
    const Fixture* GetFixtureA() const;

    /// Get the child primitive index for fixture A.
    int32_t GetChildIndexA() const;

    /// Get fixture B in this contact.
    Fixture* GetFixtureB();
    const Fixture* GetFixtureB() const;

    /// Get the child primitive index for fixture B.
    int32_t GetChildIndexB() const;

    /// Override the default friction mixture. You can call this in ContactListener::PreSolve.
    /// This value persists until set or reset.
    void SetFriction(float friction);

    /// Get the friction.
    float GetFriction() const;

    /// Reset the friction mixture to the default value.
    void ResetFriction();

    /// Override the default restitution mixture. You can call this in ContactListener::PreSolve.
    /// The value persists until you set or reset.
    void SetRestitution(float restitution);

    /// Get the restitution.
    float GetRestitution() const;

    /// Reset the restitution to the default value.
    void ResetRestitution();

    /// Override the default restitution velocity threshold mixture. You can call this in ContactListener::PreSolve.
    /// The value persists until you set or reset.
    void SetRestitutionThreshold(float threshold);

    /// Get the restitution threshold.
    float GetRestitutionThreshold() const;

    /// Reset the restitution threshold to the default value.
    void ResetRestitutionThreshold();

    /// Set the desired tangent speed for a conveyor belt behavior. In meters per second.
    void SetTangentSpeed(float speed);

    /// Get the desired tangent speed. In meters per second.
    float GetTangentSpeed() const;

    /// Evaluate this contact with your own manifold and transforms.
    virtual void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) = 0;

  protected:
    friend class ContactManager;
    friend class World;
    friend class ContactSolver;
    friend class Body;
    friend class Fixture;

    // Flags stored in m_flags
    enum {
      // Used when crawling contact graph when forming islands.
      IslandFlag    = 0x0001,

      // Set when the shapes are touching.
      TouchingFlag    = 0x0002,

      // This contact can be disabled (by user)
      EnabledFlag    = 0x0004,

      // This contact needs filtering because a fixture filter was changed.
      FilterFlag    = 0x0008,

      // This bullet contact had a TOI event
      BulletHitFlag    = 0x0010,

      // This contact has a valid TOI in m_toi
      ToiFlag      = 0x0020
    };

    /// Flag this contact for filtering. Filtering will occur the next time step.
    void FlagForFiltering();

    static void AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destroyFcn,
                        Shape::Type typeA, Shape::Type typeB);
    static void InitializeRegisters();
    static Contact* Create(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, Shape::Type typeA, Shape::Type typeB, BlockAllocator* allocator);
    static void Destroy(Contact* contact, BlockAllocator* allocator);

    Contact() : fixture_a_(nullptr), fixture_b_(nullptr) {}
    Contact(Fixture* fixtureA, int32_t indexA, Fixture* fixtureB, int32_t indexB);
    virtual ~Contact() {}

    void Update(ContactListener* listener);

    
    // --------------------------
    // Member variable
    // --------------------------
    static ContactRegister registers_[(size_t)Shape::Type::TypeCount][(size_t)Shape::Type::TypeCount];
    static bool initialized_;

    uint32_t flags_;

    // World pool and list pointers.
    Contact* prev_;
    Contact* next_;

    // Nodes for connecting bodies.
    ContactEdge node_a_;
    ContactEdge node_b_;

    Fixture* fixture_a_;
    Fixture* fixture_b_;

    int32_t index_a_;
    int32_t index_b_;

    Manifold manifold_;

    int32_t toi_count_;
    float toi_;

    float friction_;
    float restitution_;
    float restitution_threshold_;

    float tangent_speed_;
  };
  
}
