//
//  contact_listner.hpp
//  ikan
//
//  Created by Ashish . on 15/02/23.
//

#pragma once

#include "box2d/b2_fixture.h"
#include "box2d/b2_world.h"

namespace ikan {
  
  class ContactListner : public b2ContactListener {
  public:
    virtual ~ContactListner() = default;
    
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
  };
  
}
