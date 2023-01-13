//
//  world.hpp
//  ikan
//
//  Created by Ashish . on 13/01/23.
//

#pragma once

#include "world_callbacks.hpp"

namespace physics {
  
  /// The world class manages all physics entities, dynamic simulation, and asynchronous queries.
  /// The world also contains efficient memory management facilities.
  class World {
  public:
    /// Construct a world object.
    /// - Parameter gravity: grapvitty of movement
    World(const glm::vec2& gravity);

  private:
    DestructionListener* destruction_listener_ = nullptr;
  };
  
}
