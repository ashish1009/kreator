//
//  body.hpp
//  ikan
//
//  Created by Ashish . on 23/01/23.
//

#pragma once

#include "shape/shape.hpp"

namespace physics {
  
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
    float angle = 0.0f;
    /// The linear velocity of the body's origin in world co-ordinates.
    Vec2 linear_velocity;
    /// The angular velocity of the body.
    float angular_velocity = 0.0f;
    /// Linear damping is use to reduce the linear velocity. The damping parameter
    /// can be larger than 1.0f but the damping effect becomes sensitive to the
    /// time step when the damping parameter is large.
    /// Units are 1/time
    float linear_damping = 0.0f;
    /// Angular damping is use to reduce the angular velocity. The damping parameter
    /// can be larger than 1.0f but the damping effect becomes sensitive to the
    /// time step when the damping parameter is large.
    /// Units are 1/time
    float angular_damping = 0.1f;
    /// Set this flag to false if this body should never fall asleep. Note that
    /// this increases CPU usage.
    bool allow_sleep = true;
    /// Is this body initially awake or sleeping?
    bool awake = true;
    /// Should this body be prevented from rotating? Useful for characters.
    bool fixed_rotation = false;
    /// Is this a fast moving body that should be prevented from tunneling through
    /// other moving bodies? Note that all bodies are prevented from tunneling through
    /// kinematic and static bodies. This setting is only considered on dynamic bodies.
    /// @warning You should use this flag sparingly since it increases processing time.
    bool bullet = false;
    /// Does this body start out enabled?
    bool enabled= true;
    /// Use this to store application specific body data.
    utils::BodyUserData user_data;
    /// Scale the gravity applied to this body.
    float gravity_scale = 1.0f;
    
    /// This funcion logs the Body Def data
    void Log() const;
    
    static std::string GetBodyTypeString(BodyType type);
  };
  
  /// A rigid body. These are created via b2World::CreateBody.
  class Body {
  public:
    /// This function returns the mass data of the body.
    /// - Returns a struct containing the mass, inertia and center of the body.
    void GetMassData(MassData* data) const;
    
    /// This function sets the mass properties to override the mass properties of the fixtures.
    /// - Note  This changes the center of mass position.
    /// - Note  Creating or destroying fixtures can also alter the mass.
    /// - Note This function has no effect if the body isn't dynamic.
    /// - Parameter data the mass properties.
    void SetMassData(const MassData* data);
    
    /// This resets the mass properties to the sum of the mass properties of the fixtures. This normally
    /// does not need to be called unless you called SetMassData to override the mass and you later
    /// want to reset the mass.
    void ResetMassData();

  private:
    friend class World;

    // m_flags
    enum {
      island_flag         = 0x0001,
      awake_flag          = 0x0002,
      autoSleep_flag      = 0x0004,
      bullet_flag         = 0x0008,
      fixed_rotation_flag = 0x0010,
      enabled_flag        = 0x0020,
      toi_flag            = 0x0040
    };

    Body(const BodyDef* bd, World* world);
    ~Body();
    
  private:
    BodyType type_;

    uint16_t flags_;

    World* world_ = nullptr;
    Body* prev_ = nullptr;
    Body* next_ = nullptr;
    
    float mass_ = 0.0f, inverse_mass_ = 0.0f;
    float inertia_ = 0.0f, inverse_inertia_ = 0.0f; // Rotational inertia about the center of mass.
    
    Transform2D xf_;   // the body origin transform
    Sweep sweep_;      // the swept motion for CCD
    
    Vec2 linear_velocity_;
    float angular_velocity_;
  };
  
}
