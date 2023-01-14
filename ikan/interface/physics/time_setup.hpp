//
//  time_setup.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  /// Profiling data. Times are in milliseconds.
  struct Profile {
    float step;
    float collide;
    float solve;
    float solveInit;
    float solveVelocity;
    float solvePosition;
    float broadphase;
    float solveTOI;
  };
  
  /// This is an internal structure.
  struct TimeStep {
    float dt;      // time step
    float inv_dt;    // inverse time step (0 if dt == 0).
    float dtRatio;  // dt * inv_dt0
    int32_t velocityIterations;
    int32_t positionIterations;
    bool warmStarting;
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
