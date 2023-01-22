//
//  timer.hpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#pragma once

namespace physics {
  
  /// Timer for profiling. This has platform specific code and may not work on every platform.
  class Timer {
  public:
    /// Default constructor
    Timer();
    /// This function resets the timer.
    void Reset();
    /// This function returns the time since construction or the last reset.
    float GetMilliseconds() const;
    
  private:
    unsigned long long start_sec_;
    unsigned long long start_usec_;
  };
  
}
