//
//  body.cpp
//  ikan
//
//  Created by Ashish . on 23/01/23.
//

#include "body.hpp"
#include "world.hpp"

namespace physics {
  
  Body::Body(const BodyDef* bd, World* world) {
    PHYSICS_INFO("Creating Body in Physics world");
    bd->Log();
  }
  
  Body::~Body() {
    PHYSICS_INFO("Creating a Body from Physics world");
  }
  
  // -------------------------------------------
  // Body Def
  // -------------------------------------------
  std::string BodyDef::GetBodyTypeString(BodyType type) {
    switch (type) {
      case BodyType::StaticBody: return "Static";
      case BodyType::DynamicBody: return "Dynamic";
      case BodyType::KinematicBody: return "Kinametic";
        PHYSICS_ASSERT(false);
    }
  }
  
  void BodyDef::Log() const {
    PHYSICS_TRACE("  Body Type        | {0}", GetBodyTypeString(type));
    PHYSICS_TRACE("  Position         | {0} | {1}", position.x, position.y);
    PHYSICS_TRACE("  Z Angle          | {0}", angle);
    PHYSICS_TRACE("  Linear Velocity  | {0} | {1}", linear_velocity.x, linear_velocity.y);
    PHYSICS_TRACE("  Angular Velocity | {0}", angular_velocity);
    PHYSICS_TRACE("  Linear Damping   | {0}", linear_damping);
    PHYSICS_TRACE("  Angular Damping  | {0}", angular_damping);
    PHYSICS_TRACE("  Allow Sleep      | {0}", allow_sleep);
    PHYSICS_TRACE("  Is Awake         | {0}", awake);
    PHYSICS_TRACE("  Fixed Rotation   | {0}", fixed_rotation);
    PHYSICS_TRACE("  Is Bullet        | {0}", bullet);
    PHYSICS_TRACE("  Enabled          | {0}", enabled);
    PHYSICS_TRACE("  Gravity Scale    | {0}", gravity_scale);
  }
  
}
