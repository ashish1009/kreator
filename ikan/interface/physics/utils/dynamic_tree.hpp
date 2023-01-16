//
//  dynamic_tree.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics/collision/collision.hpp"
#include "physics/utils/growable_stack.hpp"

namespace physics {
  
#define NullNode (-1)

  /// A node in the dynamic tree. The client does not interact with this directly.
  struct TreeNode {
    bool IsLeaf() const {
      return child1 == NullNode;
    }
    
    /// Enlarged AABB
    AABB aabb;
    
    void* userData;
    
    union {
      int32_t parent;
      int32_t next;
    };
    
    int32_t child1;
    int32_t child2;
    
    // leaf = 0, free node = -1
    int32_t height;
    
    bool moved;
  };
  
  /// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.
  /// A dynamic tree arranges data in a binary tree to accelerate
  /// queries such as volume queries and ray casts. Leafs are proxies
  /// with an AABB. In the tree we expand the proxy AABB by _fatAABBFactor
  /// so that the proxy AABB is bigger than the client object. This allows the client
  /// object to move by small amounts without triggering a tree update.
  ///
  /// Nodes are pooled and relocatable, so we use node indices rather than pointers.
  class DynamicTree {
  public:
    /// Constructing the tree initializes the node pool.
    DynamicTree();
    
    /// Destroy the tree, freeing the node pool.
    ~DynamicTree();
    
    /// Create a proxy. Provide a tight fitting AABB and a userData pointer.
    int32_t CreateProxy(const AABB& aabb, void* userData);
    
    /// Destroy a proxy. This asserts if the id is invalid.
    void DestroyProxy(int32_t proxyId);
    
    /// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
    /// then the proxy is removed from the tree and re-inserted. Otherwise
    /// the function returns immediately.
    /// @return true if the proxy was re-inserted.
    bool MoveProxy(int32_t proxyId, const AABB& aabb1, const Vec2& displacement);
    
    /// Get proxy user data.
    /// @return the proxy user data or 0 if the id is invalid.
    void* GetUserData(int32_t proxyId) const;
    
    bool WasMoved(int32_t proxyId) const;
    void ClearMoved(int32_t proxyId);
    
    /// Get the fat AABB for a proxy.
    const AABB& GetFatAABB(int32_t proxyId) const;
    
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
    
    /// Validate this tree. For testing.
    void Validate() const;
    
    /// Compute the height of the binary tree in O(N) time. Should not be
    /// called often.
    int32_t GetHeight() const;
    
    /// Get the maximum balance of an node in the tree. The balance is the difference
    /// in height of the two children of a node.
    int32_t GetMaxBalance() const;
    
    /// Get the ratio of the sum of the node areas to the root area.
    float GetAreaRatio() const;
    
    /// Build an optimal tree. Very expensive. For testing.
    void RebuildBottomUp();
    
    /// Shift the world origin. Useful for large worlds.
    /// The shift formula is: position -= newOrigin
    /// @param newOrigin the new origin with respect to the old origin
    void ShiftOrigin(const Vec2& newOrigin);
    
  private:
    
    int32_t AllocateNode();
    void FreeNode(int32_t node);
    
    void InsertLeaf(int32_t node);
    void RemoveLeaf(int32_t node);
    
    int32_t Balance(int32_t index);
    
    int32_t ComputeHeight() const;
    int32_t ComputeHeight(int32_t nodeId) const;
    
    void ValidateStructure(int32_t index) const;
    void ValidateMetrics(int32_t index) const;
    
    int32_t root_;
    
    TreeNode* nodes_;
    int32_t node_count_;
    int32_t node_capacity_;
    
    int32_t free_list_;
    
    int32_t insertion_count_;
  };
  
  inline void* DynamicTree::GetUserData(int32_t proxyId) const {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    return nodes_[proxyId].userData;
  }
  
  inline bool DynamicTree::WasMoved(int32_t proxyId) const {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    return nodes_[proxyId].moved;
  }
  
  inline void DynamicTree::ClearMoved(int32_t proxyId) {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    nodes_[proxyId].moved = false;
  }
  
  inline const AABB& DynamicTree::GetFatAABB(int32_t proxyId) const {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    return nodes_[proxyId].aabb;
  }
  
  template <typename T>
  inline void DynamicTree::Query(T* callback, const AABB& aabb) const {
    GrowableStack<int32_t, 256> stack;
    stack.Push(root_);
    
    while (stack.GetCount() > 0) {
      int32_t nodeId = stack.Pop();
      if (nodeId == NullNode) {
        continue;
      }
      
      const TreeNode* node = nodes_ + nodeId;
      
      if (TestOverlap(node->aabb, aabb)) {
        if (node->IsLeaf()) {
          bool proceed = callback->QueryCallback(nodeId);
          if (proceed == false)
          {
            return;
          }
        }
        else {
          stack.Push(node->child1);
          stack.Push(node->child2);
        }
      }
    }
  }
  
  template <typename T>
  inline void DynamicTree::RayCast(T* callback, const RayCastInput& input) const {
    Vec2 p1 = input.p1;
    Vec2 p2 = input.p2;
    Vec2 r = p2 - p1;
    IK_ASSERT(r.LengthSquared() > 0.0f);
    r.Normalize();
    
    // v is perpendicular to the segment.
    Vec2 v = Cross(1.0f, r);
    Vec2 abs_v = Abs(v);
    
    // Separating axis for segment (Gino, p80).
    // |dot(v, p1 - c)| > dot(|v|, h)
    
    float maxFraction = input.maxFraction;
    
    // Build a bounding box for the segment.
    AABB segmentAABB; {
      Vec2 t = p1 + maxFraction * (p2 - p1);
      segmentAABB.lowerBound = Min(p1, t);
      segmentAABB.upperBound = Max(p1, t);
    }
    
    GrowableStack<int32_t, 256> stack;
    stack.Push(root_);
    
    while (stack.GetCount() > 0) {
      int32_t nodeId = stack.Pop();
      if (nodeId == NullNode) {
        continue;
      }
      
      const TreeNode* node = nodes_ + nodeId;
      
      if (TestOverlap(node->aabb, segmentAABB) == false) {
        continue;
      }
      
      // Separating axis for segment (Gino, p80).
      // |dot(v, p1 - c)| > dot(|v|, h)
      Vec2 c = node->aabb.GetCenter();
      Vec2 h = node->aabb.GetExtents();
      float separation = Abs(Dot(v, p1 - c)) - Dot(abs_v, h);
      if (separation > 0.0f) {
        continue;
      }
      
      if (node->IsLeaf()) {
        RayCastInput subInput;
        subInput.p1 = input.p1;
        subInput.p2 = input.p2;
        subInput.maxFraction = maxFraction;
        
        float value = callback->RayCastCallback(subInput, nodeId);
        
        if (value == 0.0f) {
          // The client has terminated the ray cast.
          return;
        }
        
        if (value > 0.0f) {
          // Update segment bounding box.
          maxFraction = value;
          Vec2 t = p1 + maxFraction * (p2 - p1);
          segmentAABB.lowerBound = Min(p1, t);
          segmentAABB.upperBound = Max(p1, t);
        }
      }
      else {
        stack.Push(node->child1);
        stack.Push(node->child2);
      }
    }
  }

  
}
