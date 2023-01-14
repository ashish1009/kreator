//
//  timer.hpp
//  ikan
//
//  Created by Ashish . on 15/01/23.
//

#pragma once

namespace physics {
  
  /// Timer for profiling. This has platform specific code and may not work on every platform.
  class Timer {
  public:
    /// Constructor
    Timer();
    
    /// Reset the timer.
    void Reset();
    
    /// Get the time since construction or the last reset.
    float GetMilliseconds() const;
    
  private:
    unsigned long long m_start_sec;
    unsigned long long m_start_usec;
  };
  
}
