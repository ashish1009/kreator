//
//  setting.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  void* AllocDefault(int32_t size);
  void FreeDefault(void* mem);

  /// Implement this function to use your own memory allocator.
  inline void* AllocMem(int32_t size) {
    return AllocDefault(size);
  }
  
  /// If you implement b2Alloc, you should also implement this function.
  inline void FreeMem(void* mem) {
    FreeDefault(mem);
  }
  
  /// You can define this to inject whatever data you want in Body
  struct BodyUserData {
    BodyUserData() {
      pointer = 0;
    }
    
    /// For legacy compatibility
    uintptr_t pointer;
  };
}
