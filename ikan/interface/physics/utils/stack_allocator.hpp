//
//  stack_allocator.hpp
//  ikan
//
//  Created by Ashish . on 15/01/23.
//

#pragma once

namespace physics {
  
  const int32_t StackSize = 100 * 1024;  // 100k
  const int32_t MaxStackEntries = 32;
  
  struct StackEntry {
    char* data;
    int32_t size;
    bool used_malloc;
  };
  
  // This is a stack allocator used for fast per step allocations.
  // You must nest allocate/free pairs. The code will assert
  // if you try to interleave multiple allocate/free pairs.
  class StackAllocator {
  public:
    StackAllocator();
    ~StackAllocator();
    
    void* Allocate(int32_t size);
    void Free(void* p);
    
    int32_t GetMaxAllocation() const;
    
  private:
    char data_[StackSize];
    int32_t index_;
    
    int32_t allocation_;
    int32_t max_allocation_;
    
    StackEntry entries_[MaxStackEntries];
    int32_t entry_count_;
  };

  
}
