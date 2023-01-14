//
//  body.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"
#include "settings.hpp"
#include "shape.hpp"

namespace physics {
  
  class Fixture;
  class Joint;
  class Contact;
  class Controller;
  class World;
  struct FixtureDef;
  struct JointEdge;
  struct ContactEdge;

  /// The body type.  static: zero mass, zero velocity, may be manually moved
  /// kinematic: zero mass, non-zero velocity set by user, moved by solver
  /// dynamic: positive mass, non-zero velocity determined by forces, moved by solver
  enum class BodyType : uint8_t { StaticBody = 0, KinematicBody, DynamicBody };
  
  /// A body definition holds all the data needed to construct a rigid body.
  /// You can safely re-use body definitions. Shapes are added to a body after construction.
  struct BodyDef {
    /// The body type: static, kinematic, or dynamic. Note: if a dynamic body would have zero mass, the mass is set to one.
    BodyType type = BodyType::StaticBody;
    /// The world position of the body. Avoid creating bodies at the origin since this can lead to many overlapping shapes.
    Vec2 position{0.0f, 0.0f};
    /// The world angle of the body in radians.
    float angle;
    /// The linear velocity of the body's origin in world co-ordinates.
    Vec2 linear_velocity;
    /// The angular velocity of the body.
    float angular_velocity;
    /// Linear damping is use to reduce the linear velocity. The damping parameter
    /// can be larger than 1.0f but the damping effect becomes sensitive to the
    /// time step when the damping parameter is large.
    /// Units are 1/time
    float linear_damping;
    /// Angular damping is use to reduce the angular velocity. The damping parameter
    /// can be larger than 1.0f but the damping effect becomes sensitive to the
    /// time step when the damping parameter is large.
    /// Units are 1/time
    float angular_damping;
    /// Set this flag to false if this body should never fall asleep. Note that
    /// this increases CPU usage.
    bool allow_sleep = true;
    /// Is this body initially awake or sleeping?
    bool awake = true;
    /// Should this body be prevented from rotating? Useful for characters.
    bool fixed_otation = false;
    /// Is this a fast moving body that should be prevented from tunneling through
    /// other moving bodies? Note that all bodies are prevented from tunneling through
    /// kinematic and static bodies. This setting is only considered on dynamic bodies.
    /// @warning You should use this flag sparingly since it increases processing time.
    bool bullet = false;
    /// Does this body start out enabled?
    bool enabled= true;
    /// Use this to store application specific body data.
    BodyUserData user_data;
    /// Scale the gravity applied to this body.
    float gravity_scale = 1.0f;
  };
  
  /// A rigid body. These are created via World::CreateBody.
  class Body {
  public:
    /// Creates a fixture and attach it to this body. Use this function if you need
    /// to set some fixture parameters, like friction. Otherwise you can create the
    /// fixture directly from a shape.
    /// If the density is non-zero, this function automatically updates the mass of the body.
    /// Contacts are not created until the next time step.
    /// @param def the fixture definition.
    /// @warning This function is locked during callbacks.
    Fixture* CreateFixture(const FixtureDef* def);
    
    /// Creates a fixture from a shape and attach it to this body.
    /// This is a convenience function. Use FixtureDef if you need to set parameters
    /// like friction, restitution, user data, or filtering.
    /// If the density is non-zero, this function automatically updates the mass of the body.
    /// @param shape the shape to be cloned.
    /// @param density the shape density (set to zero for static bodies).
    /// @warning This function is locked during callbacks.
    Fixture* CreateFixture(const Shape* shape, float density);
    
    /// Destroy a fixture. This removes the fixture from the broad-phase and
    /// destroys all contacts associated with this fixture. This will
    /// automatically adjust the mass of the body if the body is dynamic and the
    /// fixture has positive density.
    /// All fixtures attached to a body are implicitly destroyed when the body is destroyed.
    /// @param fixture the fixture to be removed.
    /// @warning This function is locked during callbacks.
    void DestroyFixture(Fixture* fixture);
    
    /// Set the position of the body's origin and rotation.
    /// Manipulating a body's transform may cause non-physical behavior.
    /// Note: contacts are updated on the next call to World::Step.
    /// @param position the world position of the body's local origin.
    /// @param angle the world rotation in radians.
    void SetTransform(const Vec2& position, float angle);
    
    /// Get the body transform for the body's origin.
    /// @return the world transform of the body's origin.
    const Transform& GetTransform() const;
    
    /// Get the world body origin position.
    /// @return the world position of the body's origin.
    const Vec2& GetPosition() const;
    
    /// Get the angle in radians.
    /// @return the current world rotation angle in radians.
    float GetAngle() const;
    
    /// Get the world position of the center of mass.
    const Vec2& GetWorldCenter() const;
    
    /// Get the local position of the center of mass.
    const Vec2& GetLocalCenter() const;
    
    /// Set the linear velocity of the center of mass.
    /// @param v the new linear velocity of the center of mass.
    void SetLinearVelocity(const Vec2& v);
    
    /// Get the linear velocity of the center of mass.
    /// @return the linear velocity of the center of mass.
    const Vec2& GetLinearVelocity() const;
    
    /// Set the angular velocity.
    /// @param omega the new angular velocity in radians/second.
    void SetAngularVelocity(float omega);
    
    /// Get the angular velocity.
    /// @return the angular velocity in radians/second.
    float GetAngularVelocity() const;
    
    /// Apply a force at a world point. If the force is not
    /// applied at the center of mass, it will generate a torque and
    /// affect the angular velocity. This wakes up the body.
    /// @param force the world force vector, usually in Newtons (N).
    /// @param point the world position of the point of application.
    /// @param wake also wake up the body
    void ApplyForce(const Vec2& force, const Vec2& point, bool wake);
    
    /// Apply a force to the center of mass. This wakes up the body.
    /// @param force the world force vector, usually in Newtons (N).
    /// @param wake also wake up the body
    void ApplyForceToCenter(const Vec2& force, bool wake);
    
    /// Apply a torque. This affects the angular velocity
    /// without affecting the linear velocity of the center of mass.
    /// @param torque about the z-axis (out of the screen), usually in N-m.
    /// @param wake also wake up the body
    void ApplyTorque(float torque, bool wake);
    
    /// Apply an impulse at a point. This immediately modifies the velocity.
    /// It also modifies the angular velocity if the point of application
    /// is not at the center of mass. This wakes up the body.
    /// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
    /// @param point the world position of the point of application.
    /// @param wake also wake up the body
    void ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake);
    
    /// Apply an impulse to the center of mass. This immediately modifies the velocity.
    /// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
    /// @param wake also wake up the body
    void ApplyLinearImpulseToCenter(const Vec2& impulse, bool wake);
    
    /// Apply an angular impulse.
    /// @param impulse the angular impulse in units of kg*m*m/s
    /// @param wake also wake up the body
    void ApplyAngularImpulse(float impulse, bool wake);
    
    /// Get the total mass of the body.
    /// @return the mass, usually in kilograms (kg).
    float GetMass() const;
    
    /// Get the rotational inertia of the body about the local origin.
    /// @return the rotational inertia, usually in kg-m^2.
    float GetInertia() const;
    
    /// Get the mass data of the body.
    /// @return a struct containing the mass, inertia and center of the body.
    void GetMassData(MassData* data) const;
    
    /// Set the mass properties to override the mass properties of the fixtures.
    /// Note that this changes the center of mass position.
    /// Note that creating or destroying fixtures can also alter the mass.
    /// This function has no effect if the body isn't dynamic.
    /// @param data the mass properties.
    void SetMassData(const MassData* data);
    
    /// This resets the mass properties to the sum of the mass properties of the fixtures.
    /// This normally does not need to be called unless you called SetMassData to override
    /// the mass and you later want to reset the mass.
    void ResetMassData();
    
    /// Get the world coordinates of a point given the local coordinates.
    /// @param localPoint a point on the body measured relative the the body's origin.
    /// @return the same point expressed in world coordinates.
    Vec2 GetWorldPoint(const Vec2& localPoint) const;
    
    /// Get the world coordinates of a vector given the local coordinates.
    /// @param localVector a vector fixed in the body.
    /// @return the same vector expressed in world coordinates.
    Vec2 GetWorldVector(const Vec2& localVector) const;
    
    /// Gets a local point relative to the body's origin given a world point.
    /// @param worldPoint a point in world coordinates.
    /// @return the corresponding local point relative to the body's origin.
    Vec2 GetLocalPoint(const Vec2& worldPoint) const;
    
    /// Gets a local vector given a world vector.
    /// @param worldVector a vector in world coordinates.
    /// @return the corresponding local vector.
    Vec2 GetLocalVector(const Vec2& worldVector) const;
    
    /// Get the world linear velocity of a world point attached to this body.
    /// @param worldPoint a point in world coordinates.
    /// @return the world velocity of a point.
    Vec2 GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const;
    
    /// Get the world velocity of a local point.
    /// @param localPoint a point in local coordinates.
    /// @return the world velocity of a point.
    Vec2 GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const;
    
    /// Get the linear damping of the body.
    float GetLinearDamping() const;
    
    /// Set the linear damping of the body.
    void SetLinearDamping(float linearDamping);
    
    /// Get the angular damping of the body.
    float GetAngularDamping() const;
    
    /// Set the angular damping of the body.
    void SetAngularDamping(float angularDamping);
    
    /// Get the gravity scale of the body.
    float GetGravityScale() const;
    
    /// Set the gravity scale of the body.
    void SetGravityScale(float scale);
    
    /// Set the type of this body. This may alter the mass and velocity.
    void SetType(BodyType type);
    
    /// Get the type of this body.
    BodyType GetType() const;
    
    /// Should this body be treated like a bullet for continuous collision detection?
    void SetBullet(bool flag);
    
    /// Is this body treated like a bullet for continuous collision detection?
    bool IsBullet() const;
    
    /// You can disable sleeping on this body. If you disable sleeping, the
    /// body will be woken.
    void SetSleepingAllowed(bool flag);
    
    /// Is this body allowed to sleep
    bool IsSleepingAllowed() const;
    
    /// Set the sleep state of the body. A sleeping body has very
    /// low CPU cost.
    /// @param flag set to true to wake the body, false to put it to sleep.
    void SetAwake(bool flag);
    
    /// Get the sleeping state of this body.
    /// @return true if the body is awake.
    bool IsAwake() const;
    
    /// Allow a body to be disabled. A disabled body is not simulated and cannot
    /// be collided with or woken up.
    /// If you pass a flag of true, all fixtures will be added to the broad-phase.
    /// If you pass a flag of false, all fixtures will be removed from the
    /// broad-phase and all contacts will be destroyed.
    /// Fixtures and joints are otherwise unaffected. You may continue
    /// to create/destroy fixtures and joints on disabled bodies.
    /// Fixtures on a disabled body are implicitly disabled and will
    /// not participate in collisions, ray-casts, or queries.
    /// Joints connected to a disabled body are implicitly disabled.
    /// An diabled body is still owned by a World object and remains
    /// in the body list.
    void SetEnabled(bool flag);
    
    /// Get the active state of the body.
    bool IsEnabled() const;
    
    /// Set this body to have fixed rotation. This causes the mass
    /// to be reset.
    void SetFixedRotation(bool flag);
    
    /// Does this body have fixed rotation?
    bool IsFixedRotation() const;
    
    /// Get the list of all fixtures attached to this body.
    Fixture* GetFixtureList();
    const Fixture* GetFixtureList() const;
    
    /// Get the list of all joints attached to this body.
    JointEdge* GetJointList();
    const JointEdge* GetJointList() const;
    
    /// Get the list of all contacts attached to this body.
    /// @warning this list changes during the time step and you may
    /// miss some collisions if you don't use ContactListener.
    ContactEdge* GetContactList();
    const ContactEdge* GetContactList() const;
    
    /// Get the next body in the world's body list.
    Body* GetNext();
    const Body* GetNext() const;
    
    /// Get the user data pointer that was provided in the body definition.
    BodyUserData& GetUserData();
    
    /// Get the parent world of this body.
    World* GetWorld();
    const World* GetWorld() const;
    
    /// Dump this body to a file
    void Dump();
    
  private:
    
    friend class World;
    friend class Island;
    friend class ContactManager;
    friend class ContactSolver;
    friend class Contact;
    
    friend class DistanceJoint;
    friend class FrictionJoint;
    friend class GearJoint;
    friend class MotorJoint;
    friend class MouseJoint;
    friend class PrismaticJoint;
    friend class PulleyJoint;
    friend class RevoluteJoint;
    friend class RopeJoint;
    friend class WeldJoint;
    friend class WheelJoint;
    
    // m_flags
    enum {
      e_islandFlag    = 0x0001,
      e_awakeFlag      = 0x0002,
      e_autoSleepFlag    = 0x0004,
      e_bulletFlag    = 0x0008,
      e_fixedRotationFlag  = 0x0010,
      e_enabledFlag    = 0x0020,
      e_toiFlag      = 0x0040
    };
    
    Body(const BodyDef* bd, World* world);
    ~Body();
    
    void SynchronizeFixtures();
    void SynchronizeTransform();
    
    // This is used to prevent connected bodies from colliding.
    // It may lie, depending on the collideConnected flag.
    bool ShouldCollide(const Body* other) const;
    
    void Advance(float t);
    
    // ---------------------
    // member variables
    // ---------------------
    BodyType type_;
    uint16_t flags_;
    int32_t is_land_index_;
    
    Transform xf_;    // the body origin transform
    Sweep sweep_;    // the swept motion for CCD
    
    Vec2 linear_velocity_;
    float angular_velocity_;
    
    Vec2 force_;
    float torque_;
    
    World* world_;
    Body* prev_;
    Body* next_;
    
    Fixture* fixture_list_;
    int32_t fixture_count_;
    
    JointEdge* joint_list_;
    ContactEdge* contact_list_;
    
    float mass_, inv_mass_;
    
    // Rotational inertia about the center of mass.
    float inertia_, inv_inertia_;
    
    float linear_damping_;
    float angular_damping_;
    float gravity_scale_;
    
    float sleep_time_;
    
    BodyUserData user_data_;
  };
  
}
