//
//  world_callbacks.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "world_callbacks.hpp"
#include "fixture.hpp"

namespace physics {
  
  // Return true if contact calculations should be performed between these two shapes.
  // If you implement your own collision filter you may want to build from this implementation.
  bool ContactFilter::ShouldCollide(Fixture* fixtureA, Fixture* fixtureB) {
    const Filter& filterA = fixtureA->GetFilterData();
    const Filter& filterB = fixtureB->GetFilterData();
    
    if (filterA.group_index == filterB.group_index && filterA.group_index != 0) {
      return filterA.group_index > 0;
    }
    
    bool collide = (filterA.mask_bits & filterB.category_bits) != 0 && (filterA.category_bits & filterB.mask_bits) != 0;
    return collide;
  }

  
}
