//
//  setting.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "physics/utils/settings.hpp"

namespace physics {
  
  // Memory allocators. Modify these to use your own allocator.
  void* AllocDefault(int32_t size) {
    return malloc(size);
  }
  
  void FreeDefault(void* mem) {
    free(mem);
  }

}
