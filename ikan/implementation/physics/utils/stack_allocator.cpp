//
//  stack_allocator.cpp
//  ikan
//
//  Created by Ashish . on 15/01/23.
//

#include "stack_allocator.hpp"
#include "physics_math.hpp"

namespace physics {
  
  StackAllocator::StackAllocator() {
    index_ = 0;
    allocation_ = 0;
    max_allocation_ = 0;
    entry_count_ = 0;
  }
  
  StackAllocator::~StackAllocator() {
    IK_ASSERT(index_ == 0);
    IK_ASSERT(entry_count_ == 0);
  }
  
  void* StackAllocator::Allocate(int32_t size) {
    IK_ASSERT(entry_count_ < MaxStackEntries);
    
    StackEntry* entry = entries_ + entry_count_;
    entry->size = size;
    if (index_ + size > StackSize) {
      entry->data = (char*)AllocMem(size);
      entry->used_malloc = true;
    }
    else {
      entry->data = data_ + index_;
      entry->used_malloc = false;
      index_ += size;
    }
    
    allocation_ += size;
    max_allocation_ = Max(max_allocation_, allocation_);
    ++entry_count_;
    
    return entry->data;
  }
  
  void StackAllocator::Free(void* p) {
    IK_ASSERT(entry_count_ > 0);
    StackEntry* entry = entries_ + (entry_count_ - 1);
    IK_ASSERT(p == entry->data);
    if (entry->used_malloc) {
      FreeMem(p);
    }
    else {
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
