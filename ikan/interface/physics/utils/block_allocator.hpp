//
//  block_allocator.hpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

namespace physics {
  
  const int32_t BlockSizeCount = 16;
  
  /// This Structure stores the next pointer to the block of allocated memory using physics::BlockAllocator
  struct Block {
    Block* next;
  };
  
  /// This structure stores the Block allocator chunk information
  struct Chunk {
    int32_t block_size;
    Block* blocks;
  };

  /// This class is a small object allocator used for allocating small objects that persist for more than one time step.
  /// - We are allocating memory in chunkes and using the memory in blocks for each allocations
  ///   based on the size of allocations. Storing next free block in array acc to the size of block
  /// - Important: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
  class BlockAllocator {
  public:
    /// This constructor allocate the memory chunk and set the chunk as ZERO
    BlockAllocator();
    /// This destructor delete the memory chunk
    ~BlockAllocator();

    /// This function allocates memory.
    /// - Parameter size: size of memory allocation
    void* Allocate(int32_t size);
    /// This function free memory.
    /// - Parameters:
    ///   - p: memory pointer to free
    ///   - size: size of memory to free
    void Free(void* p, int32_t size);
    
    /// This function clears all the block memory inside the chunks
    /// - Important: Chunk memory is still there
    void Clear();

  private:
    Chunk* chunks_;
    int32_t chunk_count_;
    int32_t chunk_space_;
    
    Block* free_lists_[BlockSizeCount];
  };
  
}
