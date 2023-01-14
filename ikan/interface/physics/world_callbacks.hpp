//
//  world_callbacks.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  class Fixture;
  class Joint;
  class Contact;
  struct Manifold;

  /// Joints and fixtures are destroyed when their associated body is destroyed. Implement this
  /// listener so that you may nullify references to these joints and shapes.
  class DestructionListener {
  public:
    virtual ~DestructionListener() {}
    
    /// This function Called when any joint is about to be destroyed due to the destruction of one of its attached bodies.
    virtual void SayGoodbye(Joint* joint) = 0;
    
    /// This function Called when any fixture is about to be destroyed due to the destruction of its parent body.
    virtual void SayGoodbye(Fixture* fixture) = 0;
  };
  
  class ContactFilter {
  public:
    virtual ~ContactFilter() {}
    
    /// Return true if contact calculations should be performed between these two shapes.
    /// @warning for performance reasons this is only called when the AABBs begin to overlap.
    virtual bool ShouldCollide(Fixture* fixtureA, Fixture* fixtureB);
  };

  /// Contact impulses for reporting. Impulses are used instead of forces because
  /// sub-step forces may approach infinity for rigid body collisions. These
  /// match up one-to-one with the contact points in b2Manifold.
  struct ContactImpulse {
    float normal_mpulses[MaxManifoldPoints];
    float tangent_mpulses[MaxManifoldPoints];
    int32_t count;
  };
  
  /// Implement this class to get contact information. You can use these results for
  /// things like sounds and game logic. You can also get contact results by
  /// traversing the contact lists after the time step. However, you might miss
  /// some contacts because continuous physics leads to sub-stepping.
  /// Additionally you may receive multiple callbacks for the same contact in a
  /// single time step.
  /// You should strive to make your callbacks efficient because there may be
  /// many callbacks per time step.
  /// @warning You cannot create/destroy Box2D entities inside these callbacks.
  class ContactListener {
  public:
    virtual ~ContactListener() {}
    
    /// Called when two fixtures begin to touch.
    virtual void BeginContact(Contact* contact) { NOT_USED(contact); }
    
    /// Called when two fixtures cease to touch.
    virtual void EndContact(Contact* contact) { NOT_USED(contact); }
    
    /// This is called after a contact is updated. This allows you to inspect a
    /// contact before it goes to the solver. If you are careful, you can modify the
    /// contact manifold (e.g. disable contact).
    /// A copy of the old manifold is provided so that you can detect changes.
    /// Note: this is called only for awake bodies.
    /// Note: this is called even when the number of contact points is zero.
    /// Note: this is not called for sensors.
    /// Note: if you set the number of contact points to zero, you will not
    /// get an EndContact callback. However, you may get a BeginContact callback
    /// the next step.
    virtual void PreSolve(Contact* contact, const Manifold* oldManifold) {
      NOT_USED(contact);
      NOT_USED(oldManifold);
    }
    
    /// This lets you inspect a contact after the solver is finished. This is useful
    /// for inspecting impulses.
    /// Note: the contact manifold does not include time of impact impulses, which can be
    /// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
    /// in a separate data structure.
    /// Note: this is only called for contacts that are touching, solid, and awake.
    virtual void PostSolve(Contact* contact, const ContactImpulse* impulse) {
      NOT_USED(contact);
      NOT_USED(impulse);
    }
  };
  
}
