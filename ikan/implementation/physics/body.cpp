//
//  body.cpp
//  ikan
//
//  Created by Ashish . on 23/01/23.
//

#include "body.hpp"
#include "world.hpp"

namespace physics {
  
  using namespace math;
  
  Body::Body(const BodyDef* bd, World* world)
  : world_(world) {
    PHYSICS_ASSERT(bd->position.IsValid());
    PHYSICS_ASSERT(bd->linear_velocity.IsValid());
    PHYSICS_ASSERT(IsValid(bd->angle));
    PHYSICS_ASSERT(IsValid(bd->angular_velocity));
    PHYSICS_ASSERT(IsValid(bd->angular_damping) and bd->angular_damping >= 0.0f);
    PHYSICS_ASSERT(IsValid(bd->linear_damping) and bd->linear_damping >= 0.0f);
    
    flags_ = 0;
    
    if (bd->bullet) {
      flags_ |= bullet_flag;
    }
    if (bd->fixed_rotation) {
      flags_ |= fixed_rotation_flag;
    }
    if (bd->allow_sleep) {
      flags_ |= autoSleep_flag;
    }
    if (bd->awake && bd->type != BodyType::StaticBody) {
      flags_ |= awake_flag;
    }
    if (bd->enabled) {
      flags_ |= enabled_flag;
    }

    xf_.p = bd->position;
    xf_.q.Set(bd->angle);

    sweep_.local_center.SetZero();
    sweep_.c0 = xf_.p;
    sweep_.c = xf_.p;
    sweep_.a0 = bd->angle;
    sweep_.a = bd->angle;
    sweep_.alpha0 = 0.0f;
    
    type_ = bd->type;
    
    linear_velocity_ = bd->linear_velocity;
    angular_velocity_ = bd->angular_velocity;

    PHYSICS_INFO("Creating Body in Physics world");
    bd->Log();
  }
  
  Body::~Body() {
    PHYSICS_INFO("Creating a Body from Physics world");
  }
  
  void Body::GetMassData(MassData* data) const {
    data->mass = mass_;
    data->inertia = inertia_ + mass_ * math::Dot(sweep_.local_center, sweep_.local_center);
    data->center = sweep_.local_center;
  }
  
  void Body::SetMassData(const MassData* mass_data) {
    PHYSICS_ASSERT(!world_->IsLocked());
    if (type_ != BodyType::DynamicBody) {
      return;
    }
    
    inverse_mass_ = 0.0f;
    inertia_ = 0.0f;
    inverse_inertia_ = 0.0f;
    
    mass_ = mass_data->mass;
    if (mass_ <= 0.0f) {
      mass_ = 1.0f;
    }

    inverse_mass_ = 1.0f / mass_;
    if (mass_data->inertia > 0.0f and (flags_ & Body::fixed_rotation_flag) == 0) {
      inertia_ = mass_data->inertia - mass_ * math::Dot(mass_data->center, mass_data->center);
      PHYSICS_ASSERT(inertia_ > 0.0f);
      inverse_inertia_ = 1.0f / inertia_;
    }

    // Move center of mass.
    Vec2 old_center = sweep_.c;
    sweep_.local_center =  mass_data->center;
    sweep_.c0 = sweep_.c = Mul(xf_, sweep_.local_center);
    
    // Update center of mass velocity.
    linear_velocity_ += Cross(angular_velocity_, sweep_.c - old_center);
  }
  
  void Body::ResetMassData() {
    IK_ASSERT(false);
  }
  
  void Body::SetFixedRotation(bool flag) {
    bool status = (flags_ & fixed_rotation_flag) == fixed_rotation_flag;
    if (status == flag) {
      return;
    }
    
    if (flag) {
      flags_ |= fixed_rotation_flag;
    }
    else {
      flags_ &= ~fixed_rotation_flag;
    }
    
    angular_velocity_ = 0.0f;
    
    ResetMassData();
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
