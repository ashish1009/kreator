//
//  growable_stack.hpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#pragma once

namespace physics {
  
  /// This is a growable LIFO stack with an initial capacity of N. If the stack size exceeds the initial capacity,
  /// the heap is used to increase the size of the stack.
  template <typename T, int32_t N>
  class GrowableStack {
  public:
    GrowableStack() {
      stack_ = array_;
    }
    
    ~GrowableStack() {
      if (stack_ != array_) {
        utils::Free(stack_);
        stack_ = nullptr;
      }
    }
    
    void Push(const T& element) {
      if (count_ == capacity_) {
        T* old = stack_;
        capacity_ *= 2;
        stack_ = (T*)utils::Alloc(capacity_ * sizeof(T));
        memcpy(stack_, old, count_ * sizeof(T));
        if (old != array_) {
          utils::Free(old);
        }
      }
      
      stack_[count_] = element;
      ++count_;
    }
    
    T Pop() {
      PHYSICS_ASSERT(count_ > 0);
      --count_;
      return stack_[count_];
    }
    
    int32_t GetCount() {
      return count_;
    }

  private:
    T* stack_;
    T array_[N];
    int32_t count_ = 0;
    int32_t capacity_ = N;
  };
  
}
