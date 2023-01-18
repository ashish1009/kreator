//
//  time_step.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

namespace ikan {
  
  /// This is a class to store the time difference between two frames. NOTE: This class only store the
  /// time stamp, it is not any counter to update the time on its own Time need to be updated each
  /// frame by either window or application
  class Timestep {
  public:
    /// This constructor creates the instance and store the time
    /// - Parameter time: Initial time to be set:
    Timestep(float time = 0.0f);
    /// Defatult construtor
    ~Timestep() = default;
    
    /// This function returns the Time as float
    operator float() const;
    
    /// This function returns the time step (Time took to render 1 Frame) in Seconds
    float Seconds() const;
    /// This function returns the time step (Time took to render 1 Frame) in mili Seconds
    float MilliSeconds() const;
    /// This function returns the time step (Time took to render 1 Frame) in micro Seconds
    float MicroSeconds() const;
    
  private:
    float time_ = 0.0f;
  };
  
} 
