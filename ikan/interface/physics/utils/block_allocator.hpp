//
//  block_allocator.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  const int32_t BlockSizeCount = 14;

  struct Block;
  struct Chunk;
  
  /// This is a small object allocator used for allocating small objects that persist for more than one time step.
  /// See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
  class BlockAllocator {
  public:
    BlockAllocator();
    ~BlockAllocator();
    
    /// Allocate memory. This will use b2Alloc if the size is larger than b2_maxBlockSize.
    void* Allocate(int32_t size);
    
    /// Free memory. This will use b2Free if the size is larger than b2_maxBlockSize.
    void Free(void* p, int32_t size);
    
    void Clear();
    
  private:
    
    Chunk* chunks_;
    int32_t chunk_count_;
    int32_t chunk_space_;
    
    Block* free_lists_[BlockSizeCount];
  };

}
