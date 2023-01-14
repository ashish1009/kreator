//
//  world.hpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#pragma once

#include "physics_math.hpp"
#include "contact_manager.hpp"

namespace physics {
  
  class DestructionListener;
  class Draw;
  class Body;
  
  /// The world class manages all physics entities, dynamic simulation, and asynchronous queries.
  /// The world also contains efficient memory management facilities.
  class World {
  public:
    /// Construct a world object.
    /// - Parameter gravity: grapvitty of movement
    World(const Vec2& gravity);

  private:
    friend class Body;
    friend class Fixture;
    friend class ContactManager;
    friend class Controller;

    DestructionListener* destruction_listener_ = nullptr;
    Draw* debug_draw_ = nullptr;

    Body* body_list_;
    
    ContactManager contact_manager;
  };
  
}
