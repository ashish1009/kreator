//
//  board_phase.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "broad_phase.hpp"
#include "settings.hpp"

namespace physics {
  
  BroadPhase::BroadPhase() {
    proxy_count_ = 0;
    
    pair_capacity_ = 16;
    pair_count_ = 0;
    pair_buffer_ = (Pair*)AllocMem(pair_capacity_ * sizeof(Pair));
    
    move_capacity_ = 16;
    move_count_ = 0;
    move_buffer_ = (int32_t*)AllocMem(move_capacity_ * sizeof(int32_t));
  }
  
  BroadPhase::~BroadPhase() {
    FreeMem(move_buffer_);
    FreeMem(pair_buffer_);
  }
  
  int32_t BroadPhase::CreateProxy(const AABB& aabb, void* userData) {
    int32_t proxyId = tree_.CreateProxy(aabb, userData);
    ++proxy_count_;
    BufferMove(proxyId);
    return proxyId;
  }
  
  void BroadPhase::DestroyProxy(int32_t proxyId) {
    UnBufferMove(proxyId);
    --proxy_count_;
    tree_.DestroyProxy(proxyId);
  }
  
  void BroadPhase::MoveProxy(int32_t proxyId, const AABB& aabb, const Vec2& displacement){
    bool buffer = tree_.MoveProxy(proxyId, aabb, displacement);
    if (buffer) {
      BufferMove(proxyId);
    }
  }
  
  void BroadPhase::TouchProxy(int32_t proxyId) {
    BufferMove(proxyId);
  }
  
  void BroadPhase::BufferMove(int32_t proxyId) {
    if (move_count_ == move_capacity_) {
      int32_t* oldBuffer = move_buffer_;
      move_capacity_ *= 2;
      move_buffer_ = (int32_t*)AllocMem(move_capacity_ * sizeof(int32_t));
      memcpy(move_buffer_, oldBuffer, move_count_ * sizeof(int32_t));
      FreeMem(oldBuffer);
    }
    
    move_buffer_[move_count_] = proxyId;
    ++move_count_;
  }
  
  void BroadPhase::UnBufferMove(int32_t proxyId) {
    for (int32_t i = 0; i < move_count_; ++i) {
      if (move_buffer_[i] == proxyId) {
        move_buffer_[i] = NullProxy;
      }
    }
  }
  
  // This is called from DynamicTree::Query when we are gathering pairs.
  bool BroadPhase::QueryCallback(int32_t proxyId) {
    // A proxy cannot form a pair with itself.
    if (proxyId == query_proxyId_) {
      return true;
    }
    
    const bool moved = tree_.WasMoved(proxyId);
    if (moved && proxyId > query_proxyId_) {
      // Both proxies are moving. Avoid duplicate pairs.
      return true;
    }
    
    // Grow the pair buffer as needed.
    if (pair_count_ == pair_capacity_) {
      Pair* oldBuffer = pair_buffer_;
      pair_capacity_ = pair_capacity_ + (pair_capacity_ >> 1);
      pair_buffer_ = (Pair*)AllocMem(pair_capacity_ * sizeof(Pair));
      memcpy(pair_buffer_, oldBuffer, pair_count_ * sizeof(Pair));
      FreeMem(oldBuffer);
    }
    
    pair_buffer_[pair_count_].proxy_IdA = Min(proxyId, query_proxyId_);
    pair_buffer_[pair_count_].proxy_IdB = Max(proxyId, query_proxyId_);
    ++pair_count_;
    
    return true;
  }
  
}
