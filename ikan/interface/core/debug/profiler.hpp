//
//  profiler.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the profiler interface API

// Call 'Profile()' in begining of any scope. It will compute the time and print
// the Profiler result at the end of Scope
#define PROFILE() ikan::Profiler _profiler(__PRETTY_FUNCTION__);

namespace ikan {
  
  /// This Class is responsible to get the execution time of a scope. Intantiate the Profiler with function
  /// name in any scope. It will print the executrion time of the function. or just call the Macro defiled PROFILE().
  class Profiler {
  public:
    /// This Constructor instantiate the function name that need to be profiled
    /// - Parameter function_name: Function name to be profiled
    Profiler(const char* function_name);
    
    /// Profiler destructor
    ~Profiler();
    
  private:
    DELETE_COPY_MOVE_CONSTRUCTORS(Profiler);
    
    std::chrono::time_point<std::chrono::steady_clock> start_time_point_;
    std::chrono::time_point<std::chrono::steady_clock> end_tIme_point_;
    std::chrono::duration<float> duration_;
    const char* function_name_;
  };
  
} // namespace ikan
