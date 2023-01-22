//
//  world.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#pragma once

namespace physics {
  
  /// The world class manages all physics entities, dynamic simulation, and asynchronous queries.
  /// The world also contains efficient memory management facilities.
  class World {
  public:
    /// Construct a world object.
    /// - Parameter gravity: the world gravity vector.
    World(const Vec2& gravity);
    
    /// Destruct the world. All physics entities are destroyed and all heap memory is released.
    ~World();

  private:
    Vec2 gravity_;
  };
  
}
