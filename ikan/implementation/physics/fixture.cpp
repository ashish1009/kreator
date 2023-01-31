//
//  fixture.cpp
//  ikan
//
//  Created by Ashish . on 31/01/23.
//

#include "fixture.hpp"

namespace physics {
  
  void FixtureDef::Log() {
    PHYSICS_TRACE("Friction               | {0}", friction);
    PHYSICS_TRACE("Restitution            | {0}", restitution);
    PHYSICS_TRACE("Restitution Threshold  | {0}", restitution_threshold);
    PHYSICS_TRACE("Density                | {0}", density);
    PHYSICS_TRACE("Is Sensor              | {0}", is_sensor);
  }
  
}
