//
//  physics_time_step.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#pragma once

#include "physics/utils/physics_math.hpp"

namespace physics {
  
  /// Profiling data. Times are in milliseconds.
  struct Profile {
    float step;
    float collide;
    float solve;
    float solve_init;
    float solve_velocity;
    float solve_position;
    float broadphase;
    float solve_toi;
  };
  
  /// This is an internal structure.
  struct TimeStep {
    float dt;      // time step
    float inv_dt;    // inverse time step (0 if dt == 0).
    float dt_ratio;  // dt * inv_dt0
    int32_t velocity_iterations;
    int32_t position_iterations;
    bool warm_starting;
  };
  
  /// This is an internal structure.
  struct Position {
    Vec2 c;
    float a;
  };
  
  /// This is an internal structure.
  struct Velocity {
    Vec2 v;
    float w;
  };
  
  /// Solver Data
  struct SolverData {
    TimeStep step;
    Position* positions;
    Velocity* velocities;
  };
  
}