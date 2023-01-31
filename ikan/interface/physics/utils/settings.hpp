//
//  settings.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
  
  /// You can use this to change the length scale used by your game. For example for inches you could use 39.4.
#define LengthUnitsPerMeter     1.0f
  
  /// The maximum number of vertices on a convex polygon. You cannot increase this too much because
  /// BlockAllocator has a maximum object size.
#define MaxPolygonVertices      8

  namespace utils {
    // User data
    
    /// You can define this to inject whatever data you want in b2Body
    struct BodyUserData {
      uintptr_t pointer;
    };
    
    /// You can define this to inject whatever data you want in b2Fixture
    struct FixtureUserData {
      uintptr_t pointer;
    };

    // Default allocation and free API
    void* AllocImpl(int32_t size);
    void FreeImpl(void* mem);

    /// This function allocate the memory of size
    /// - Parameter size: size of memory allocation
    /// - Important: Change Implementation for User Defined Allocation
    inline void* Alloc(int32_t size) {
      return AllocImpl(size);
    }
    /// This function delete the allocated memory
    /// - Parameter mem: memory pointer to be deleted
    /// - Important: Change Implementation for User Defined Free
    inline void Free(void* mem) {
      FreeImpl(mem);
    }
    
  }
  
}
