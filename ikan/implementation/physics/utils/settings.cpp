//
//  settings.cpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#include "settings.hpp"

namespace physics {
  
  namespace utils {

    void* AllocImpl(int32_t size) {
      return malloc(size);
    }
    void FreeImpl(void* mem) {
      free(mem);
    }

  }
  
}
