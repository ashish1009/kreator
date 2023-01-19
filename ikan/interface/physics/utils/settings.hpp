//
//  settings.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
  
  namespace utils {
    
    /// This function allocate the memory of size
    /// - Parameter size: size of memory allocation
    void* Alloc(int32_t size);
    /// This function delete the allocated memory
    /// - Parameter mem: memory pointer to be deleted
    void Free(void* mem);
    
  }
  
}
