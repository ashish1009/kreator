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
  
  inline void* BroadPhase::GetUserData(int32_t proxyId) const {
    return tree_.GetUserData(proxyId);
  }
  
  inline bool BroadPhase::TestOverlap(int32_t proxyIdA, int32_t proxyIdB) const {
    const AABB& aabbA = tree_.GetFatAABB(proxyIdA);
    const AABB& aabbB = tree_.GetFatAABB(proxyIdB);
    return physics::TestOverlap(aabbA, aabbB);
  }
  
  inline const AABB& BroadPhase::GetFatAABB(int32_t proxyId) const {
    return tree_.GetFatAABB(proxyId);
  }
  
  inline int32_t BroadPhase::GetProxyCount() const {
    return proxy_count_;
  }
  
  inline int32_t BroadPhase::GetTreeHeight() const {
    return tree_.GetHeight();
  }
  
  inline int32_t BroadPhase::GetTreeBalance() const {
    return tree_.GetMaxBalance();
  }
  
  inline float BroadPhase::GetTreeQuality() const {
    return tree_.GetAreaRatio();
  }
  
  template <typename T>
  void BroadPhase::UpdatePairs(T* callback) {
    // Reset pair buffer
    pair_count_ = 0;
    
    // Perform tree queries for all moving proxies.
    for (int32_t i = 0; i < move_count_; ++i) {
      query_proxyId_ = move_buffer_[i];
      if (query_proxyId_ == NullProxy) {
        continue;
      }
      
      // We have to query the tree with the fat AABB so that
      // we don't fail to create a pair that may touch later.
      const AABB& fatAABB = tree_.GetFatAABB(query_proxyId_);
      
      // Query tree, create pairs and add them pair buffer.
      tree_.Query(this, fatAABB);
    }
    
    // Send pairs to caller
    for (int32_t i = 0; i < pair_count_; ++i) {
      Pair* primaryPair = pair_buffer_ + i;
      void* userDataA = tree_.GetUserData(primaryPair->proxy_IdA);
      void* userDataB = tree_.GetUserData(primaryPair->proxy_IdB);
      
      callback->AddPair(userDataA, userDataB);
    }
    
    // Clear move flags
    for (int32_t i = 0; i < move_count_; ++i) {
      int32_t proxyId = move_buffer_[i];
      if (proxyId == NullProxy) {
        continue;
      }
      
      tree_.ClearMoved(proxyId);
    }
    
    // Reset move buffer
    move_count_ = 0;
  }
  
  template <typename T>
  inline void BroadPhase::Query(T* callback, const AABB& aabb) const {
    tree_.Query(callback, aabb);
  }
  
  template <typename T>
  inline void BroadPhase::RayCast(T* callback, const RayCastInput& input) const {
    tree_.RayCast(callback, input);
  }
  
  inline void BroadPhase::ShiftOrigin(const Vec2& newOrigin) {
    tree_.ShiftOrigin(newOrigin);
  }
  
  
}
