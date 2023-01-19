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
    void* userData;
    
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
  private:
    // Member Variables
    TreeNode* nodes_;
    
    int32_t root_;
    int32_t node_count_;
    int32_t node_capacity_;
    int32_t free_list_;
    int32_t insertion_count_;
  };
  
}
