//
//  dynamic_tree.hpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#pragma once

#include "physics/collision/collision.hpp"

namespace physics {
  
#define NullNode (-1)
  
  /// A node in the dynamic tree. The client does not interact with this directly.
  struct TreeNode {
    bool IsLeaf() const { return child_1 == NullNode; }

    AABB aabb;
    void* user_data;
    
    union {
      int32_t parent;
      int32_t next;
    };
    
    int32_t child_1;
    int32_t child_2;
    int32_t height;
    
    bool moved;
  };
  
  /// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt. A dynamic tree
  /// arranges data in a binary tree to accelerate queries such as volume queries and ray casts. Leafs
  /// are proxies with an AABB. In the tree we expand the proxy AABB by FatAABBFactor so that the
  /// proxy AABB is bigger than the client object. This allows the client object to move by small amounts
  /// without triggering a tree update.
  /// - Important: Nodes are pooled and relocatable, so we use node indices rather than pointers.
  class DynamicTree {
  public:
    /// This constructore allocates memory for tree
    DynamicTree();
    /// Destroy the memory
    ~DynamicTree();
    
    /// This function creates a proxy. Provide a tight fitting AABB and a user_data pointer.
    /// - Parameters:
    ///   - aabb: AABB Bounding box
    ///   - user_data: user data
    /// - Important: Create a proxy in the tree as a leaf node. We return the index of the node
    ///              instead of a pointer so that we can grow the node pool.
    int32_t CreateProxy(const AABB& aabb, void* user_data);
    /// This function destroys a proxy. This asserts if the id is invalid.
    /// - Parameter proxy_id: Proxy Id to be destroyed
    void DestroyProxy(int32_t proxy_id);

    /// This function moves a proxy with a swepted AABB. If the proxy has moved outside of its fattened
    /// AABB, then the proxy is removed from the tree and re-inserted. Otherwise the function returns
    /// immediately.
    /// - Returns true if the proxy was re-inserted.
    /// - Parameters:
    ///   - proxyId: proxy ID
    ///   - aabb1: aabb bounding box
    ///   - displacement: displacement vector
    bool MoveProxy(int32_t proxy_id, const AABB& aabb1, const Vec2& displacement);

    /// This function returns Get proxy user data.
    /// - Returns: the proxy user data or 0 if the id is invalid.
    /// - Parameter proxy_id: Proxy ID
    void* GetUserData(int32_t proxy_id) const;
    /// Thit function check is proxy moved
    /// - Parameter proxy_id: Proxy ID
    bool WasMoved(int32_t proxy_id) const;
    /// This function clear the move for a proxy
    /// - Parameter proxy_id: Proxy ID
    void ClearMoved(int32_t  proxy_id);
    
    /// This function returnst the fat AABB for a proxy.
    /// - Parameter proxy_id: Proxy ID
    const AABB& GetFatAABB(int32_t  proxy_id) const;
    
    /// This function queries an AABB for overlapping proxies. The callback class is called for each
    /// proxy that overlaps the supplied AABB.
    /// - Parameters:
    ///   - callback: Callback registered
    ///   - aabb: aabb bouding box
    template <typename T> void Query(T* callback, const AABB& aabb) const;
    
    /// This function Ray-cast against the proxies in the tree. This relies on the callback to perform a
    /// exact ray-cast in the case were the proxy contains a shape. The callback also performs the
    /// any collision filtering. This has performance roughly equal to k * log(n), where k is the number
    /// of collisions and n is the number of proxies in the tree.
    /// - Parameters:
    ///   - input: the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
    ///   - callback: a callback class that is called for each proxy that is hit by the ray.
    template <typename T> void RayCast(T* callback, const RayCastInput& input) const;
    
    /// This function validates this tree. For testing.
    void Validate() const;
    
    /// This function computes the height of the binary tree in O(N) time. Should not be called often.
    int32_t  GetHeight() const;
    
    /// This function returns the maximum balance of an node in the tree. The balance is the difference
    /// in height of the two children of a node.
    int32_t  GetMaxBalance() const;
    
    /// This function return the ratio of the sum of the node areas to the root area.
    float GetAreaRatio() const;
    
    /// This function builds an optimal tree. Very expensive. For testing.
    void RebuildBottomUp();
    
    /// This function shifts the world origin. Useful for large worlds The shift formula is: position -= newOrigin
    /// - Parameter newOrigin: the new origin with respect to the old origin
    void ShiftOrigin(const Vec2& newOrigin);
    
  private:
    // Member functions
    /// Allocate a node from the pool. Grow the pool if necessary. Returns the node ID
    int32_t AllocateNode();
    /// This function free the node with ID
    /// - Parameter node_id: node id to be deleted
    /// - Important: Return a node to the pool.
    void FreeNode(int32_t node_id);
    
    /// This function inset the leaf to the tree
    /// - Parameter node_id: node id
    void InsertLeaf(int32_t node_id);
    /// This function remove the leaf from the tree
    /// - Parameter node_id: node id
    void RemoveLeaf(int32_t node_id);
    
    /// This function performs a left or right rotation if node A is imbalanced. Returns the new root index.
    /// - Parameter index: index which need to be balanced
    int32_t Balance(int32_t index);
    
    /// This function compute the height of tree
    int32_t ComputeHeight() const;
    /// This function compute the height of tree
    /// - Parameter node_id: node id
    int32_t ComputeHeight(int32_t node_id) const;
    
    /// This function validates the structure
    /// - Parameter index: node id
    void ValidateStructure(int32_t index) const;
    /// This function validates the matrix
    /// - Parameter index: node id
    void ValidateMetrics(int32_t index) const;

    // Member Variables
    TreeNode* nodes_;
    
    int32_t root_ = NullNode;
    int32_t node_count_ = 0;
    int32_t node_capacity_ = 16;
    int32_t free_list_ = 0;
    int32_t insertion_count_ = 0;
  };
  
}
