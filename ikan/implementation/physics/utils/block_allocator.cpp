//
//  block_allocator.cpp
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#include "block_allocator.hpp"

namespace physics {
  
  static const int32_t ChunkSize = 16 * 1024;
  static const int32_t ChunkArrayIncrement = 128;
  static const int32_t MaxBlockSize = 640;
  
  // These are the supported object sizes. Actual allocations are rounded up the next size.
  static const int32_t BlockSizes[BlockSizeCount] = { 16, 32, 64, 96, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640 };

  // This structture maps an arbitrary allocation size to a suitable slot in BlockSizes.
  struct SizeMap {
    SizeMap() {
      values[0] = 0;
      for (int32_t j = 0, i = 1; i <= MaxBlockSize; ++i) {
        PHYSICS_ASSERT(j < BlockSizeCount);
        if (i <= BlockSizes[j]) {
          values[i] = (uint8_t)j;
        }
        else {
          ++j;
          values[i] = (uint8_t)j;
        }
      }
    }
    
    uint8_t values[MaxBlockSize + 1];
  };
  static const SizeMap SizeMap;

  BlockAllocator::BlockAllocator() {
    PHYSICS_ASSERT(BlockSizeCount < UCHAR_MAX);
    
    chunk_space_ = ChunkArrayIncrement;
    chunk_count_ = 0;
    chunks_ = (Chunk*)utils::Alloc(chunk_space_ * sizeof(Chunk));
    
    memset(chunks_, 0, chunk_space_ * sizeof(Chunk));
    memset(free_lists_, 0, sizeof(free_lists_));
  }
  
  BlockAllocator::~BlockAllocator() {
    for (int32_t i = 0; i < chunk_count_; ++i)
      utils::Free(chunks_[i].blocks);
    
    utils::Free(chunks_);
  }

  void* BlockAllocator::Allocate(int32_t size) {
    PHYSICS_ASSERT(0 < size);
    
    if (size == 0)
      return nullptr;
    
    if (size > MaxBlockSize)
      return utils::Alloc(size);

    int32_t index = SizeMap.values[size];
    PHYSICS_ASSERT(0 <= index and index < BlockSizeCount);

    if (free_lists_[index]) {
      Block* block = free_lists_[index];
      free_lists_[index] = block->next;
      return block;
    } else {
      // If we used all the chunks. Increase the memory
      if (chunk_count_ == chunk_space_) {
        Chunk* old_chunks = chunks_;
        chunk_space_ += ChunkArrayIncrement;
        chunks_ = (Chunk*)utils::Alloc(chunk_space_ * sizeof(Chunk));
        memcpy(chunks_, old_chunks, chunk_count_ * sizeof(Chunk));
        memset(chunks_ + chunk_count_, 0, ChunkArrayIncrement * sizeof(Chunk));
        utils::Free(old_chunks);
      }

      Chunk* chunk = chunks_ + chunk_count_;
      chunk->blocks = (Block*)utils::Alloc(ChunkSize);

      int32_t block_size = BlockSizes[index];
      chunk->block_size = block_size;

      int32_t block_count = ChunkSize / block_size;
      PHYSICS_ASSERT(block_count * block_size <= ChunkSize);

      for (int32_t i = 0; i < block_count - 1; ++i) {
        Block* block = (Block*)((int8_t*)chunk->blocks + block_size * i);
        Block* next = (Block*)((int8_t*)chunk->blocks + block_size * (i + 1));
        block->next = next;
      }
      
      Block* last = (Block*)((int8_t*)chunk->blocks + block_size * (block_count - 1));
      last->next = nullptr;

      free_lists_[index] = chunk->blocks->next;
      ++chunk_count_;

      return chunk->blocks;
    }
    PHYSICS_ASSERT(false);
    return nullptr;
  }
  
  void BlockAllocator::Free(void* p, int32_t size) {
    if (size == 0)
      return;

    PHYSICS_ASSERT(0 < size);

    if (size > MaxBlockSize) {
      utils::Free(p);
      return;
    }

    int32_t index = SizeMap.values[size];
    PHYSICS_ASSERT(0 <= index and index < BlockSizeCount);

    // Verify the memory address and size is valid.
    int32_t block_size = BlockSizes[index];
    bool found = false;
    for (int32_t i = 0; i < chunk_count_; ++i) {
      Chunk* chunk = chunks_ + i;
      if (chunk->block_size != block_size) {
        PHYSICS_ASSERT((int8_t*)p + block_size <= (int8_t*)chunk->blocks or (int8_t*)chunk->blocks + ChunkSize <= (int8_t*)p);
      } else {
        if ((int8_t*)chunk->blocks <= (int8_t*)p and (int8_t*)p + block_size <= (int8_t*)chunk->blocks + ChunkSize) {
          found = true;
        }
      }
    }
    PHYSICS_ASSERT(found);
    memset(p, 0xfd, block_size);
    
    Block* block = (Block*)p;
    block->next = free_lists_[index];
    free_lists_[index] = block;
  }

  void BlockAllocator::Clear() {
    for (int32_t i = 0; i < chunk_count_; ++i) {
      utils::Free(chunks_[i].blocks);
    }
    
    chunk_count_ = 0;
    memset(chunks_, 0, chunk_space_ * sizeof(Chunk));
    memset(free_lists_, 0, sizeof(free_lists_));
  }

  
}
