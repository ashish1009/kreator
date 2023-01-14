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
  };
  
}
