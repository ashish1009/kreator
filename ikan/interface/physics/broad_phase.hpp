//
//  board_phase.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "dynamic_tree.hpp"

namespace physics {
  
  struct Pair {
    int32_t proxy_IdA;
    int32_t proxy_IdB;
  };
  
  /// The broad-phase is used for computing pairs and performing volume queries and ray casts.
  /// This broad-phase does not persist pairs. Instead, this reports potentially new pairs.
  /// It is up to the client to consume the new pairs and to track subsequent overlap.
  class BroadPhase {
  public:
    enum { NullProxy = -1 };
    
    BroadPhase();
    ~BroadPhase();
    
    /// Create a proxy with an initial AABB. Pairs are not reported until
    /// UpdatePairs is called.
    int32_t CreateProxy(const AABB& aabb, void* userData);
    
    /// Destroy a proxy. It is up to the client to remove any pairs.
    void DestroyProxy(int32_t proxyId);
    
    /// Call MoveProxy as many times as you like, then when you are done
    /// call UpdatePairs to finalized the proxy pairs (for your time step).
    void MoveProxy(int32_t proxyId, const AABB& aabb, const Vec2& displacement);
    
    /// Call to trigger a re-processing of it's pairs on the next call to UpdatePairs.
    void TouchProxy(int32_t proxyId);
    
    /// Get the fat AABB for a proxy.
    const AABB& GetFatAABB(int32_t proxyId) const;
    
    /// Get user data from a proxy. Returns nullptr if the id is invalid.
    void* GetUserData(int32_t proxyId) const;
    
    /// Test overlap of fat AABBs.
    bool TestOverlap(int32_t proxyIdA, int32_t proxyIdB) const;
    
    /// Get the number of proxies.
    int32_t GetProxyCount() const;
    
    /// Update the pairs. This results in pair callbacks. This can only add pairs.
    template <typename T>
    void UpdatePairs(T* callback);
    
    /// Query an AABB for overlapping proxies. The callback class
    /// is called for each proxy that overlaps the supplied AABB.
    template <typename T>
    void Query(T* callback, const AABB& aabb) const;
    
    /// Ray-cast against the proxies in the tree. This relies on the callback
    /// to perform a exact ray-cast in the case were the proxy contains a shape.
    /// The callback also performs the any collision filtering. This has performance
    /// roughly equal to k * log(n), where k is the number of collisions and n is the
    /// number of proxies in the tree.
    /// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
    /// @param callback a callback class that is called for each proxy that is hit by the ray.
    template <typename T>
    void RayCast(T* callback, const RayCastInput& input) const;
    
    /// Get the height of the embedded tree.
    int32_t GetTreeHeight() const;
    
    /// Get the balance of the embedded tree.
    int32_t GetTreeBalance() const;
    
    /// Get the quality metric of the embedded tree.
    float GetTreeQuality() const;
    
    /// Shift the world origin. Useful for large worlds.
    /// The shift formula is: position -= newOrigin
    /// @param newOrigin the new origin with respect to the old origin
    void ShiftOrigin(const Vec2& newOrigin);
    
  private:
    
    friend class DynamicTree;
    
    void BufferMove(int32_t proxyId);
    void UnBufferMove(int32_t proxyId);
    
    bool QueryCallback(int32_t proxyId);
    
    DynamicTree tree_;
    
    int32_t proxy_count_;
    
    int32_t* move_buffer_;
    int32_t move_capacity_;
    int32_t move_count_;
    
    Pair* pair_buffer_;
    int32_t pair_capacity_;
    int32_t pair_count_;
    
    int32_t query_proxyId_;
  };
  
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
