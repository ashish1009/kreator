//
//  block_allocator.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "block_allocator.hpp"
#include "settings.hpp"

namespace physics {
  
  static const int32_t ChunkSize = 16 * 1024;
  static const int32_t MaxBlockSize = 640;
  static const int32_t ChunkArrayIncrement = 128;
  
  // These are the supported object sizes. Actual allocations are rounded up the next size.
  static const int32_t blockSizes[BlockSizeCount] = {
    16,    // 0
    32,    // 1
    64,    // 2
    96,    // 3
    128,  // 4
    160,  // 5
    192,  // 6
    224,  // 7
    256,  // 8
    320,  // 9
    384,  // 10
    448,  // 11
    512,  // 12
    640,  // 13
  };
  
  // This maps an arbitrary allocation size to a suitable slot in blockSizes.
  struct SizeMap {
    SizeMap() {
      int32_t j = 0;
      values[0] = 0;
      for (int32_t i = 1; i <= MaxBlockSize; ++i) {
        IK_ASSERT(j < BlockSizeCount);
        if (i <= blockSizes[j]) {
          values[i] = (uint8_t)j;
        } else {
          ++j;
          values[i] = (uint8_t)j;
        }
      }
    }
    
    uint8_t values[MaxBlockSize + 1];
  };
  
  static const SizeMap sizeMap;
  
  struct Chunk {
    int32_t block_size;
    Block* blocks;
  };
  
  struct Block {
    Block* next;
  };
  
  BlockAllocator::BlockAllocator()
  {
    IK_ASSERT(BlockSizeCount < UCHAR_MAX);
    
    chunk_space_ = ChunkArrayIncrement;
    chunk_count_ = 0;
    chunks_ = (Chunk*)AllocMem(chunk_space_ * sizeof(Chunk));
    
    memset(chunks_, 0, chunk_space_ * sizeof(Chunk));
    memset(free_lists_, 0, sizeof(free_lists_));
  }
  
  BlockAllocator::~BlockAllocator() {
    for (int32_t i = 0; i < chunk_count_; ++i) {
      FreeMem(chunks_[i].blocks);
    }
    
    FreeMem(chunks_);
  }
  
  void* BlockAllocator::Allocate(int32_t size) {
    if (size == 0)
      return nullptr;
    
    IK_ASSERT(0 < size);
    
    if (size > MaxBlockSize) {
      return AllocMem(size);
    }
    
    int32_t index = sizeMap.values[size];
    IK_ASSERT(0 <= index && index < BlockSizeCount);
    
    if (free_lists_[index]) {
      Block* block = free_lists_[index];
      free_lists_[index] = block->next;
      return block;
    } else {
      if (chunk_count_ == chunk_space_) {
        Chunk* oldChunks = chunks_;
        chunk_space_ += ChunkArrayIncrement;
        chunks_ = (Chunk*)AllocMem(chunk_space_ * sizeof(Chunk));
        memcpy(chunks_, oldChunks, chunk_count_ * sizeof(Chunk));
        memset(chunks_ + chunk_count_, 0, ChunkArrayIncrement * sizeof(Chunk));
        FreeMem(oldChunks);
      }
      
      Chunk* chunk = chunks_ + chunk_count_;
      chunk->blocks = (Block*)AllocMem(ChunkSize);
#if defined(IK_DEBUG_FEATURE)
      memset(chunk->blocks, 0xcd, ChunkSize);
#endif
      int32_t block_size = blockSizes[index];
      chunk->block_size = block_size;
      int32_t block_count = ChunkSize / block_size;
      IK_ASSERT(block_count * block_size <= ChunkSize);
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
  }
  
  void BlockAllocator::Free(void* p, int32_t size) {
    if (size == 0)
      return;
    
    IK_ASSERT(0 < size);
    
    if (size > MaxBlockSize) {
      FreeMem(p);
      return;
    }
    
    int32_t index = sizeMap.values[size];
    IK_ASSERT(0 <= index && index < BlockSizeCount);
    
#if defined(IK_DEBUG_FEATURE)
    // Verify the memory address and size is valid.
    int32_t block_size = blockSizes[index];
    bool found = false;
    for (int32_t i = 0; i < chunk_count_; ++i)
    {
      Chunk* chunk = chunks_ + i;
      if (chunk->block_size != block_size) {
        IK_ASSERT((int8_t*)p + block_size <= (int8_t*)chunk->blocks ||
                 (int8_t*)chunk->blocks + ChunkSize <= (int8_t*)p);
      } else {
        if ((int8_t*)chunk->blocks <= (int8_t*)p && (int8_t*)p + block_size <= (int8_t*)chunk->blocks + ChunkSize)
        {
          found = true;
        }
      }
    }
    
    IK_ASSERT(found);
    
    memset(p, 0xfd, block_size);
#endif
    
    Block* block = (Block*)p;
    block->next = free_lists_[index];
    free_lists_[index] = block;
  }
  
  void BlockAllocator::Clear() {
    for (int32_t i = 0; i < chunk_count_; ++i) {
      FreeMem(chunks_[i].blocks); }
    
    chunk_count_ = 0;
    memset(chunks_, 0, chunk_space_ * sizeof(Chunk));
    memset(free_lists_, 0, sizeof(free_lists_));
  }

}
