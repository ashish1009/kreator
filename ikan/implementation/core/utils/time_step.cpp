//
//  time_step.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "time_step.hpp"

namespace ikan {
  
  Timestep::Timestep(float time) : time_(time) {}
  Timestep::operator float() const { return time_; }
  
  float Timestep::Seconds() const { return time_; }
  float Timestep::MilliSeconds() const { return time_ * 1000.0f; }
  float Timestep::MicroSeconds() const { return time_ * 1000000.0f; }
  
} 
