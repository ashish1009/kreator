//
//  settings.cpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#include "settings.hpp"

namespace physics {
  
  namespace utils {
    
    void* Alloc(int32_t size) {
      return malloc(size);
    }
    void Free(void* mem) {
      free(mem);
    }
    
  }
  
}
