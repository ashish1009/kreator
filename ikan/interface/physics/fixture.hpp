//
//  fixture.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "shape.hpp"
#include "body.hpp"
#include "physics_math.hpp"

namespace physics {
  
  class BroadPhase;
  class Fixture;

  /// This holds contact filtering data.
  struct Filter {
    Filter() {
      category_bits = 0x0001;
      mask_bits = 0xFFFF;
      group_index = 0;
    }
    
    /// The collision category bits. Normally you would just set one bit.
    uint16_t category_bits;
    
    /// The collision mask bits. This states the categories that this shape would accept for collision.
    uint16_t mask_bits;
    
    /// Collision groups allow a certain group of objects to never collide (negative) or always collide
    /// (positive). Zero means no collision group. Non-zero group filtering always wins against the mask bits.
    int16_t group_index;
  };
  
  /// This proxy is used internally to connect fixtures to the broad-phase.
  struct FixtureProxy {
    AABB aabb;
    Fixture* fixture;
    int32_t child_index;
    int32_t proxy_id;
  };
  
  /// A fixture definition is used to create a fixture. This class defines a abstract fixture definition. You can reuse fixture definitions safely.
  struct FixtureDef {
    /// The constructor sets the default fixture definition values.
    FixtureDef() {
      shape = nullptr;
      friction = 0.2f;
      restitution = 0.0f;
      restitution_yhreshold = 1.0f * LengthUnitsPerMeter;
      density = 0.0f;
      is_sensor = false;
    }
    
    /// The shape, this must be set. The shape will be cloned, so you can create the shape on the stack.
    const Shape* shape;
    
    /// Use this to store application specific fixture data.
    FixtureUserData userData;
    
    /// The friction coefficient, usually in the range [0,1].
    float friction;
    
    /// The restitution (elasticity) usually in the range [0,1].
    float restitution;
    
    /// Restitution velocity threshold, usually in m/s. Collisions above this
    /// speed have restitution applied (will bounce).
    float restitution_yhreshold;
    
    /// The density, usually in kg/m^2.
    float density;
    
    /// A sensor shape collects contact information but never generates a collision
    /// response.
    bool is_sensor;
    
    /// Contact filtering data.
    Filter filter;
  };
  
  /// A fixture is used to attach a shape to a body for collision detection. A fixture inherits its transform
  /// from its parent. Fixtures hold additional non-geometric data such as friction, collision filters, etc.
  /// Fixtures are created via Body::CreateFixture.
  /// @warning you cannot reuse fixtures.
  class Fixture {
    /// Get the type of the child shape. You can use this to down cast to the concrete shape.
    Shape::Type GetType() const;

    /// Get the child shape. You can modify the child shape, however you should not change the
    /// number of vertices because this will crash some collision caching mechanisms.
    /// Manipulating the shape may lead to non-physical behavior.
    Shape* GetShape();
    const Shape* GetShape() const;

    /// Set if this fixture is a sensor.
    void SetSensor(bool sensor);
    
    /// Is this fixture a sensor (non-solid)?
    bool IsSensor() const;
    
    /// Set the contact filtering data. This will not update contacts until the next time
    /// step when either parent body is active and awake.
    /// This automatically calls Refilter.
    void SetFilterData(const Filter& filter);
    
    /// Get the contact filtering data.
    const Filter& GetFilterData() const;
    
    /// Call this if you want to establish collision that was previously disabled by ContactFilter::ShouldCollide.
    void Refilter();
    
    /// Get the parent body of this fixture. This is nullptr if the fixture is not attached.
    Body* GetBody();
    const Body* GetBody() const;

    /// Get the next fixture in the parent body's fixture list.
    Fixture* GetNext();
    const Fixture* GetNext() const;

    /// Get the user data that was assigned in the fixture definition. Use this to
    /// store your application specific data.
    FixtureUserData& GetUserData();
    
    /// Test a point for containment in this fixture.
    /// - Parameter p: a point in world coordinates.
    bool TestPoint(const Vec2& p) const;
    
    /// Cast a ray against this shape.
    /// - Parameters:
    ///   - output: the ray-cast results.
    ///   - input: the ray-cast input parameters.
    ///   - childIndex: the child shape index (e.g. edge index)
    bool RayCast(RayCastOutput* output, const RayCastInput& input, int32_t childIndex) const;
    
    /// Get the mass data for this fixture. The mass data is based on the density and
    /// the shape. The rotational inertia is about the shape's origin. This operation
    /// may be expensive.
    void GetMassData(MassData* massData) const;
    
    /// Set the density of this fixture. This will _not_ automatically adjust the mass
    /// of the body. You must call Body::ResetMassData to update the body's mass.
    void SetDensity(float density);
    
    /// Get the density of this fixture.
    float GetDensity() const;
    
    /// Get the coefficient of friction.
    float GetFriction() const;
    
    /// Set the coefficient of friction. This will _not_ change the friction of
    /// existing contacts.
    void SetFriction(float friction);
    
    /// Get the coefficient of restitution.
    float GetRestitution() const;
    
    /// Set the coefficient of restitution. This will _not_ change the restitution of
    /// existing contacts.
    void SetRestitution(float restitution);
    
    /// Get the restitution velocity threshold.
    float GetRestitutionThreshold() const;
    
    /// Set the restitution threshold. This will _not_ change the restitution threshold of
    /// existing contacts.
    void SetRestitutionThreshold(float threshold);
    
    /// Get the fixture's AABB. This AABB may be enlarge and/or stale.
    /// If you need a more accurate AABB, compute it using the shape and
    /// the body transform.
    const AABB& GetAABB(int32_t childIndex) const;
    
    /// Dump this fixture to the log file.
    void Dump(int32_t bodyIndex);
    
  protected:
    
    friend class Body;
    friend class World;
    friend class Contact;
    friend class ContactManager;
    friend class CircleContact;
    friend class PolygonAndCircleContact;
    friend class PolygonContact;
    friend class EdgeAndCircleContact;
    friend class EdgeAndPolygonContact;
    friend class ChainAndCircleContact;
    friend class ChainAndPolygonContact;
    friend class ContactFilter;
    
    Fixture();
    
    // We need separation create/destroy functions from the constructor/destructor because
    // the destructor cannot access the allocator (no destructor arguments allowed by C++).
    void Create(BlockAllocator* allocator, Body* body, const FixtureDef* def);
    void Destroy(BlockAllocator* allocator);
    
    // These support body activation/deactivation.
    void CreateProxies(BroadPhase* broadPhase, const Transform& xf);
    void DestroyProxies(BroadPhase* broadPhase);
    
    void Synchronize(BroadPhase* broadPhase, const Transform& xf1, const Transform& xf2);
    
    float density_;
    
    Fixture* next_;
    Body* body_;
    
    Shape* shape_;
    
    float friction_;
    float restitution_;
    float restitution_threshold_;
    
    FixtureProxy* proxies_;
    int32_t proxy_count_;
    
    Filter filter_;
    
    bool is_sensor_;
    
    FixtureUserData user_data_;
  };
  

}
