//
//  settings.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
  
  namespace utils {
    
    // Default allocation and free API
    void* AllocImpl(int32_t size);
    void FreeImpl(void* mem);

    /// This function allocate the memory of size
    /// - Parameter size: size of memory allocation
    /// - Important: Change Implementation for User Defined Allocation
    void* Alloc(int32_t size) {
      return AllocImpl(size);
    }
    /// This function delete the allocated memory
    /// - Parameter mem: memory pointer to be deleted
    /// - Important: Change Implementation for User Defined Free
    void Free(void* mem) {
      FreeImpl(mem);
    }
    
  }
  
}
