//
//  profiler.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "profiler.hpp"

namespace ikan {
  
  Profiler::Profiler(const char* function_name) : function_name_(function_name) {
    start_time_point_ = std::chrono::high_resolution_clock::now();
  }
  
  Profiler::~Profiler() {
    end_tIme_point_ = std::chrono::high_resolution_clock::now();;
    duration_ = end_tIme_point_ - start_time_point_;
    
#ifdef IK_DEBUG_FEATURE
    IK_TRACE("  Execution time for ");
    IK_TRACE("  | Function      | {0}", function_name_);
    IK_TRACE("  | seconds       | {0}", duration_.count());
    IK_TRACE("  | mili seconds  | {0}", duration_.count() * 1000);
    IK_TRACE("  | micro seconds | {0}", duration_.count() * 1000000);
#else
    std::cout << "  Execution time for \n";
    std::cout << "  | Function      | " << function_name_ << "\n";
    std::cout << "  | seconds       | " << duration_.count() << "\n";
    std::cout << "  | mili seconds  | " << duration_.count() * 1000 << "\n";
    std::cout << "  | micro seconds | " << duration_.count() * 1000000 << "\n";
#endif
  }
  
}
