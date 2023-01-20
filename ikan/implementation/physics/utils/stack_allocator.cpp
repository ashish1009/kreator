//
//  stack_allocator.cpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#include "stack_allocator.hpp"

namespace physics {
  
  StackAllocator::~StackAllocator() {
    // Avoid Memory Leaks
    PHYSICS_ASSERT(index_ == 0);
    PHYSICS_ASSERT(entry_count_ == 0);
  }

  void* StackAllocator::Allocate(int32_t size) {
    PHYSICS_ASSERT(entry_count_ < MaxStackEntries);
    
    StackEntry* entry = entries_ + entry_count_;
    entry->size = size;
    if (index_ + size > StackSize) {
      entry->data = (std::byte*)utils::Alloc(size);
      entry->used_malloc = true;
    } else {
      entry->data = data_ + index_;
      entry->used_malloc = false;
      index_ += size;
    }

    allocation_ += size;
    max_allocation_ = math::Max(max_allocation_, allocation_);
    ++entry_count_;
    
    return entry->data;
  }
  
  void StackAllocator::Free(void* p) {
    PHYSICS_ASSERT(entry_count_ > 0);
    StackEntry* entry = entries_ + entry_count_ - 1;
    
    PHYSICS_ASSERT(p == entry->data);
    if (entry->used_malloc) {
      utils::Free(p);
    } else {
      index_ -= entry->size;
    }
    allocation_ -= entry->size;
    --entry_count_;
    
    p = nullptr;
  }
  
  int32_t StackAllocator::GetMaxAllocation() const {
    return max_allocation_;
  }
  
}
