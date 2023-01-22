//
//  timer.cpp
//  ikan
//
//  Created by Ashish . on 22/01/23.
//

#include "physics_timer.hpp"

#include <sys/time.h>

namespace physics {

  Timer::Timer() {
    Reset();
  }
  
  void Timer::Reset() {
    timeval t;
    gettimeofday(&t, 0);
    start_sec_ = t.tv_sec;
    start_usec_ = t.tv_usec;
  }
  
  float Timer::GetMilliseconds() const {
    timeval t;
    gettimeofday(&t, 0);
    time_t start_sec = start_sec_;
    suseconds_t start_usec = (suseconds_t)start_usec_;
    
    // http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
    if (t.tv_usec < start_usec) {
      int nsec = (start_usec - t.tv_usec) / 1000000 + 1;     start_usec -= 1000000 * nsec;
      start_sec += nsec;
    }
    
    if (t.tv_usec - start_usec > 1000000) {
      int nsec = (t.tv_usec - start_usec) / 1000000;
      start_usec += 1000000 * nsec;
      start_sec -= nsec;
    }
    return 1000.0f * (t.tv_sec - start_sec) + 0.001f * (t.tv_usec - start_usec);
  }
  
}
