//
//  time_of_impact.hpp
//  ikan
//
//  Created by Ashish . on 17/01/23.
//

#pragma once

#include "collision/distance.hpp"

namespace physics {
  
  /// Input parameters for TimeOfImpact
  struct TOIInput {
    DistanceProxy proxyA;
    DistanceProxy proxyB;
    Sweep sweepA;
    Sweep sweepB;
    float tMax;    // defines sweep interval [0, tMax]
  };
  
  /// Output parameters for TimeOfImpact.
  struct TOIOutput {
    enum State {
      e_unknown,
      e_failed,
      e_overlapped,
      e_touching,
      e_separated
    };
    
    State state;
    float t;
  };
  
  /// Compute the upper bound on time before two shapes penetrate. Time is represented as
  /// a fraction between [0,tMax]. This uses a swept separating axis and may miss some intermediate,
  /// non-tunneling collisions. If you change the time interval, you should call this function
  /// again.
  /// Note: use Distance to compute the contact point and normal at the time of impact.
  void TimeOfImpact(TOIOutput* output, const TOIInput* input);
  
}
