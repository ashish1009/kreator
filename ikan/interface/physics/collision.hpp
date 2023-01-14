//
//  collision.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  /// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
  struct RayCastInput {
    Vec2 p1, p2;
    float maxFraction;
  };
  
  /// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2 come from RayCastInput.
  struct RayCastOutput {
    Vec2 normal;
    float fraction;
  };
  
}
