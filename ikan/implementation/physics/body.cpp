//
//  body.cpp
//  ikan
//
//  Created by Ashish . on 23/01/23.
//

#include "body.hpp"
#include "world.hpp"

namespace physics {
  
  std::string BodyDef::GetBodyTypeString(BodyType type) {
    switch (type) {
      case BodyType::StaticBody: return "Static";
      case BodyType::DynamicBody: return "Dynamic";
      case BodyType::KinematicBody: return "Kinametic";
      PHYSICS_ASSERT(false);
    }
  }
  
  void BodyDef::Log() const {
    PHYSICS_INFO("Body Type        | {0}", GetBodyTypeString(type));
    PHYSICS_INFO("Position         | {0} | {1}", position.x, position.y);
    PHYSICS_INFO("Z Angle          | {0}", angle);
    PHYSICS_INFO("Linear Velocity  | {0} | {1}", linear_velocity.x, linear_velocity.y);
    PHYSICS_INFO("Angular Velocity | {0}", angular_velocity);
    PHYSICS_INFO("Linear Damping   | {0}", linear_damping);
    PHYSICS_INFO("Angular Damping  | {0}", angular_damping);
    PHYSICS_INFO("Allow Sleep      | {0}", allow_sleep);
    PHYSICS_INFO("Is Awake         | {0}", awake);
    PHYSICS_INFO("Fixed Rotation   | {0}", fixed_rotation);
    PHYSICS_INFO("Is Bullet        | {0}", bullet);
    PHYSICS_INFO("Enabled          | {0}", enabled);
    PHYSICS_INFO("Gravity Scale    | {0}", gravity_scale);
  }
  
  Body::Body(const BodyDef* bd, World* world) {
    PHYSICS_INFO("Creating Body in Physics world");
    bd->Log();
  }
  
  Body::~Body() {
    PHYSICS_INFO("Creating a Body from Physics world");
  }
  
}
