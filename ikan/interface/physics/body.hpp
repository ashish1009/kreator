//
//  body.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"
#include "settings.hpp"

namespace physics {
  
  struct ContactEdge;
  struct JointEdge;
  class World;

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
    /// Get the body transform for the body's origin.
    /// @return the world transform of the body's origin.
    const Transform& GetTransform() const;
    
    /// Get the list of all contacts attached to this body.
    /// @warning this list changes during the time step and you may
    /// miss some collisions if you don't use b2ContactListener.
    ContactEdge* GetContactList();
    const ContactEdge* GetContactList() const;
    
    /// Set the sleep state of the body. A sleeping body has very
    /// low CPU cost.
    /// @param flag set to true to wake the body, false to put it to sleep.
    void SetAwake(bool flag);
    /// Get the sleeping state of this body.
    /// @return true if the body is awake.
    bool IsAwake() const;

    /// Get the parent world of this body.
    World* GetWorld();
    const World* GetWorld() const;
    
    // This is used to prevent connected bodies from colliding.
    // It may lie, depending on the collideConnected flag.
    bool ShouldCollide(const Body* other) const;
    
    /// Get the total mass of the body.
    /// @return the mass, usually in kilograms (kg).
    float GetMass() const;
    
    /// Get the rotational inertia of the body about the local origin.
    /// @return the rotational inertia, usually in kg-m^2.
    float GetInertia() const;
    
    /// Gets a local point relative to the body's origin given a world point.
    /// @param worldPoint a point in world coordinates.
    /// @return the corresponding local point relative to the body's origin.
    Vec2 GetLocalPoint(const Vec2& worldPoint) const;

    /// Get the world coordinates of a point given the local coordinates.
    /// @param localPoint a point on the body measured relative the the body's origin.
    /// @return the same point expressed in world coordinates.
    Vec2 GetWorldPoint(const Vec2& localPoint) const;

    /// Gets a local vector given a world vector.
    /// @param worldVector a vector in world coordinates.
    /// @return the corresponding local vector.
    Vec2 GetLocalVector(const Vec2& worldVector) const;

    /// Get the world coordinates of a vector given the local coordinates.
    /// @param localVector a vector fixed in the body.
    /// @return the same vector expressed in world coordinates.
    Vec2 GetWorldVector(const Vec2& localVector) const;

    /// Get the world body origin position.
    /// @return the world position of the body's origin.
    const Vec2& GetPosition() const;

    /// Get the angle in radians.
    /// @return the current world rotation angle in radians.
    float GetAngle() const;
    
    /// Get the active state of the body.
    bool IsEnabled() const;

  private:
    friend class DistanceJoint;
    
    // m_flags
    enum
    {
      islandFlag    = 0x0001,
      awakeFlag      = 0x0002,
      autoSleepFlag    = 0x0004,
      bulletFlag    = 0x0008,
      fixedRotationFlag  = 0x0010,
      enabledFlag    = 0x0020,
      toiFlag      = 0x0040
    };

    friend class ContactManager;
    friend class MouseJoint;
    friend class PrismaticJoint;
    friend class RevoluteJoint;
    friend class PulleyJoint;
    friend class GearJoint;
    friend class WheelJoint;
    friend class WeldJoint;
    friend class FrictionJoint;
    friend class MotorJoint;
    
    BodyType type_;
    ContactEdge* contact_list_;
    uint16_t flags_;

    float sleep_time_;

    Vec2 linear_velocity_;
    float angular_velocity_;
    Vec2 force_;
    float torque_;

    Transform xf_;
    Sweep sweep_;    // the swept motion for CCD

    World* world_;
    JointEdge* joint_list_;
    
    float inertia_, inv_inertia_;
    float mass_, inv_mass_;
    
    int32_t island_index_;
  };
  
}
