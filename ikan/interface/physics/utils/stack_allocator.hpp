//
//  stack_allocator.hpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#pragma once

namespace physics {
  
  const int32_t StackSize = 100 * 1024;  // 100k
  const int32_t MaxStackEntries = 32;
  
  /// This structure stores the stack entry data
  struct StackEntry {
    std::byte* data;
    int32_t size;
    bool used_malloc;
  };

  /// This is a stack allocator used for fast per step allocations.
  /// - Important: You must nest allocate/free pairs. The code will assert if you try to interleave multiple allocate/free pairs.
  class StackAllocator {
  public:
    StackAllocator() = default;
    ~StackAllocator();
    
    /// This function allocate the memory from stack
    /// - Parameter size: size of memory
    void* Allocate(int32_t size);
    /// This function free up the mmory used in stack
    /// - Parameter p: memory pointer
    void Free(void* p);
    
    /// This function returns the max allocations
    int32_t GetMaxAllocation() const;

  private:
    std::byte data_[StackSize];
    int32_t index_ = 0;
    
    int32_t allocation_ = 0;
    int32_t max_allocation_ = 0;
    
    StackEntry entries_[MaxStackEntries];
    int32_t entry_count_ = 0;
  };
  
}
