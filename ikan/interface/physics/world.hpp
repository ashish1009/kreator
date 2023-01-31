//
//  world.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#pragma once

#include "utils/block_allocator.hpp"
#include "utils/stack_allocator.hpp"

namespace physics {
  
  struct BodyDef;
  class Body;
  
  /// The world class manages all physics entities, dynamic simulation, and asynchronous queries.
  /// The world also contains efficient memory management facilities.
  class World {
  public:
    // -----------------------------
    // Constructor and Destructors
    // -----------------------------
    /// Construct a world object.
    /// - Parameter gravity: the world gravity vector.
    World(const Vec2& gravity);
    /// Destruct the world. All physics entities are destroyed and all heap memory is released.
    ~World();
    
    // ---------------------
    // Fundamentals
    // ---------------------
    /// Create a rigid body given a definition. No reference to the definition is retained.
    /// - Warning This function is locked during callbacks.
    Body* CreateBody(const BodyDef* def);
    /// Destroy a rigid body given a definition. No reference to the definition is retained. This function
    /// is locked during callbacks.
    /// - Warning This automatically deletes all associated shapes and joints.
    /// - Warning This function is locked during callbacks.
    void DestroyBody(Body* body);
    
    // --------------------------
    // Getters
    // --------------------------
    /// This function check is word is locked to add more body?
    bool IsLocked() const;
    
  private:
    BlockAllocator block_allocator_;
    Body* body_list_ = nullptr;
    int32_t body_count_ = 0;

    Vec2 gravity_;
    bool locked_ = false;
  };
  
}
