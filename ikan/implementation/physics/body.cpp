//
//  body.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "body.hpp"
#include "contact.hpp"
#include "joint.hpp"

namespace physics {
  
  ContactEdge* Body::GetContactList() {
    return contact_list_;
  }

  const ContactEdge* Body::GetContactList() const {
    return contact_list_;
  }
  
  void Body::SetAwake(bool flag) {
    if (type_ == BodyType::StaticBody) {
      return;
    }
    
    if (flag) {
      flags_ |= awakeFlag;
      sleep_time_ = 0.0f;
    } else {
      flags_ &= ~awakeFlag;
      sleep_time_ = 0.0f;
      linear_velocity_.SetZero();
      angular_velocity_ = 0.0f;
      force_.SetZero();
      torque_ = 0.0f;
    }
  }
  
  bool Body::IsAwake() const {
    return (flags_ & awakeFlag) == awakeFlag;
  }
  
  const Transform& Body::GetTransform() const {
    return xf_;
  }

  bool Body::ShouldCollide(const Body* other) const {
    // At least one body should be dynamic.
    if (type_ != BodyType::DynamicBody && other->type_ != BodyType::DynamicBody) {
      return false;
    }
    
    // Does a joint prevent collision?
    for (JointEdge* jn = joint_list_; jn; jn = jn->next) {
      if (jn->other == other) {
        if (jn->joint->collide_connected_ == false) {
          return false;
        }
      }
    }
    
    return true;
  }

  float Body::GetMass() const {
    return mass_;
  }
  
  float Body::GetInertia() const {
    return inertia_ + mass_ * Dot(sweep_.localCenter, sweep_.localCenter);
  }
  
  World* Body::GetWorld() {
    return world_;
  }
  
  const World* Body::GetWorld() const {
    return world_;
  }

  Vec2 Body::GetLocalPoint(const Vec2& worldPoint) const {
    return MulT(xf_, worldPoint);
  }
  
  Vec2 Body::GetWorldPoint(const Vec2& localPoint) const {
    return Mul(xf_, localPoint);
  }
  
  Vec2 Body::GetLocalVector(const Vec2& worldVector) const {
    return MulT(xf_.q, worldVector);
  }
  
  float Body::GetAngle() const {
    return sweep_.a;
  }
  
  Vec2 Body::GetWorldVector(const Vec2& localVector) const {
    return Mul(xf_.q, localVector);
  }
  
  const Vec2& Body::GetPosition() const
  {
    return xf_.p;
  }
  bool Body::IsEnabled() const
  {
    return (flags_ & enabledFlag) == enabledFlag;
  }

}
