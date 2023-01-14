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
      m_flags |= awakeFlag;
      m_sleepTime = 0.0f;
    } else {
      m_flags &= ~awakeFlag;
      m_sleepTime = 0.0f;
      m_linearVelocity.SetZero();
      m_angularVelocity = 0.0f;
      m_force.SetZero();
      m_torque = 0.0f;
    }
  }
  
  bool Body::IsAwake() const {
    return (m_flags & awakeFlag) == awakeFlag;
  }
  
  const Transform& Body::GetTransform() const {
    return m_xf;
  }

  bool Body::ShouldCollide(const Body* other) const {
    // At least one body should be dynamic.
    if (type_ != BodyType::DynamicBody && other->type_ != BodyType::DynamicBody) {
      return false;
    }
    
    // Does a joint prevent collision?
    for (JointEdge* jn = m_jointList; jn; jn = jn->next) {
      if (jn->other == other) {
        if (jn->joint->m_collideConnected == false) {
          return false;
        }
      }
    }
    
    return true;
  }

  float Body::GetMass() const {
    return m_mass;
  }
  
  float Body::GetInertia() const {
    return m_I + m_mass * Dot(m_sweep.localCenter, m_sweep.localCenter);
  }

}
