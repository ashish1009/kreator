//
//  dynamic_tree.cpp
//  ikan
//
//  Created by Ashish . on 19/01/23.
//

#include "dynamic_tree.hpp"

namespace physics {
  
  using namespace math;
  
  DynamicTree::DynamicTree() {
    nodes_ = (TreeNode*)utils::Alloc(node_capacity_ * sizeof(TreeNode));
    memset(nodes_, 0, node_capacity_ * sizeof(TreeNode));
    
    // Build a linked list for the free list.
    for (int32_t i = 0; i < node_capacity_ - 1; ++i) {
      nodes_[i].next = i + 1;
      nodes_[i].height = -1;
    }
    nodes_[node_capacity_-1].next = NullNode;
    nodes_[node_capacity_-1].height = -1;
  }
  
  DynamicTree::~DynamicTree() {
    // This frees the entire tree in one shot.
    utils::Free(nodes_);
  }
  
  int32_t DynamicTree::CreateProxy(const AABB& aabb, void* user_data) {
    int32_t proxy_id = AllocateNode();
    
    // Fatten the aabb.
    Vec2 r(AabbExtension, AabbExtension);
    nodes_[proxy_id].aabb.lower_bound = aabb.lower_bound - r;
    nodes_[proxy_id].aabb.upper_bound = aabb.upper_bound + r;
    nodes_[proxy_id].user_data = user_data;
    nodes_[proxy_id].height = 0;
    nodes_[proxy_id].moved = true;
    
    InsertLeaf(proxy_id);
    
    return proxy_id;
  }
  
  void DynamicTree::DestroyProxy(int32_t proxy_id) {
    PHYSICS_ASSERT(0 <= proxy_id and proxy_id < node_capacity_);
    PHYSICS_ASSERT(nodes_[proxy_id].IsLeaf());
    
    RemoveLeaf(proxy_id);
    FreeNode(proxy_id);
  }
  
  bool DynamicTree::MoveProxy(int32_t proxyId, const AABB& aabb, const Vec2& displacement) {
    PHYSICS_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    
    PHYSICS_ASSERT(nodes_[proxyId].IsLeaf());
    
    // Extend AABB
    AABB fatAABB;
    Vec2 r(AabbExtension, AabbExtension);
    fatAABB.lower_bound = aabb.lower_bound - r;
    fatAABB.upper_bound = aabb.upper_bound + r;
    
    // Predict AABB movement
    Vec2 d = AabbMultiplier * displacement;
    
    if (d.x < 0.0f) {
      fatAABB.lower_bound.x += d.x;
    } else {
      fatAABB.upper_bound.x += d.x;
    }
    
    if (d.y < 0.0f) {
      fatAABB.lower_bound.y += d.y;
    } else {
      fatAABB.upper_bound.y += d.y;
    }
    
    const AABB& treeAABB = nodes_[proxyId].aabb;
    if (treeAABB.Contains(aabb)) {
      // The tree AABB still contains the object, but it might be too large.
      // Perhaps the object was moving fast but has since gone to sleep.
      // The huge AABB is larger than the new fat AABB.
      AABB hugeAABB;
      hugeAABB.lower_bound = fatAABB.lower_bound - 4.0f * r;
      hugeAABB.upper_bound = fatAABB.upper_bound + 4.0f * r;
      
      if (hugeAABB.Contains(treeAABB)) {
        // The tree AABB contains the object AABB and the tree AABB is
        // not too large. No tree update needed.
        return false;
      }
      
      // Otherwise the tree AABB is huge and needs to be shrunk
    }
    
    RemoveLeaf(proxyId);
    
    nodes_[proxyId].aabb = fatAABB;
    
    InsertLeaf(proxyId);
    
    nodes_[proxyId].moved = true;
    
    return true;
  }


  int32_t DynamicTree::AllocateNode() {
    // Expand the node pool as needed.
    if (free_list_ == NullNode) {
      PHYSICS_ASSERT(node_count_ == node_capacity_);
      
      // The free list is empty. Rebuild a bigger pool.
      TreeNode* old_nodes = nodes_;
      node_capacity_ *= 2;
      nodes_ = (TreeNode*)utils::Alloc(node_capacity_ * sizeof(TreeNode));
      memcpy(nodes_, old_nodes, node_count_ * sizeof(TreeNode));
      utils::Free(old_nodes);
      
      // Build a linked list for the free list. The parent
      // pointer becomes the "next" pointer.
      for (int32_t i = node_count_; i < node_capacity_ - 1; ++i) {
        nodes_[i].next = i + 1;
        nodes_[i].height = -1;
      }
      nodes_[node_capacity_-1].next = NullNode;
      nodes_[node_capacity_-1].height = -1;
      free_list_ = node_count_;
    }
    
    // Peel a node off the free list.
    int32_t node_id = free_list_;
    free_list_ = nodes_[node_id].next;
    
    nodes_[node_id].parent = NullNode;
    nodes_[node_id].child_1 = NullNode;
    nodes_[node_id].child_2 = NullNode;
    nodes_[node_id].height = 0;
    nodes_[node_id].user_data = nullptr;
    nodes_[node_id].moved = false;
    
    ++node_count_;
    
    return node_id;
  }
  
  void DynamicTree::FreeNode(int32_t node_id) {
    PHYSICS_ASSERT(0 <= node_id and node_id < node_capacity_);
    PHYSICS_ASSERT(0 < node_count_);
    nodes_[node_id].next = free_list_;
    nodes_[node_id].height = -1;
    free_list_ = node_id;
    --node_count_;
  }
  
  void DynamicTree::InsertLeaf(int32_t leaf) {
    ++insertion_count_;
    
    if (root_ == NullNode) {
      root_ = leaf;
      nodes_[root_].parent = NullNode;
      return;
    }
    
    // Find the best sibling for this node
    AABB leaf_AABB = nodes_[leaf].aabb;
    int32_t index = root_;
    while (nodes_[index].IsLeaf() == false) {
      int32_t child_1 = nodes_[index].child_1;
      int32_t child_2 = nodes_[index].child_2;
      
      float area = nodes_[index].aabb.GetPerimeter();
      
      AABB combined_AABB;
      combined_AABB.Combine(nodes_[index].aabb, leaf_AABB);
      float combined_area = combined_AABB.GetPerimeter();
      
      // Cost of creating a new parent for this node and the new leaf
      float cost = 2.0f * combined_area;
      
      // Minimum cost of pushing the leaf further down the tree
      float inheritance_cost = 2.0f * (combined_area - area);
      
      // Cost of descending into child_1
      float cost_1;
      if (nodes_[child_1].IsLeaf()) {
        AABB aabb;
        aabb.Combine(leaf_AABB, nodes_[child_1].aabb);
        cost_1 = aabb.GetPerimeter() + inheritance_cost;
      } else {
        AABB aabb;
        aabb.Combine(leaf_AABB, nodes_[child_1].aabb);
        float old_area = nodes_[child_1].aabb.GetPerimeter();
        float new_area = aabb.GetPerimeter();
        cost_1 = (new_area - old_area) + inheritance_cost;
      }
      
      // Cost of descending into child_2
      float cost_2;
      if (nodes_[child_2].IsLeaf()) {
        AABB aabb;
        aabb.Combine(leaf_AABB, nodes_[child_2].aabb);
        cost_2 = aabb.GetPerimeter() + inheritance_cost;
      } else {
        AABB aabb;
        aabb.Combine(leaf_AABB, nodes_[child_2].aabb);
        float old_area = nodes_[child_2].aabb.GetPerimeter();
        float new_area = aabb.GetPerimeter();
        cost_2 = new_area - old_area + inheritance_cost;
      }
      
      // Descend according to the minimum cost.
      if (cost < cost_1 and cost < cost_2) {
        break;
      }
      
      // Descend
      if (cost_1 < cost_2) {
        index = child_1;
      } else {
        index = child_2;
      }
    }
    
    int32_t sibling = index;
    
    // Create a new parent.
    int32_t old_parent = nodes_[sibling].parent;
    int32_t new_parent = AllocateNode();
    nodes_[new_parent].parent = old_parent;
    nodes_[new_parent].user_data = nullptr;
    nodes_[new_parent].aabb.Combine(leaf_AABB, nodes_[sibling].aabb);
    nodes_[new_parent].height = nodes_[sibling].height + 1;
    
    if (old_parent != NullNode) {
      // The sibling was not the root.
      if (nodes_[old_parent].child_1 == sibling) {
        nodes_[old_parent].child_1 = new_parent;
      } else {
        nodes_[old_parent].child_2 = new_parent;
      }
      
      nodes_[new_parent].child_1 = sibling;
      nodes_[new_parent].child_2 = leaf;
      nodes_[sibling].parent = new_parent;
      nodes_[leaf].parent = new_parent;
    } else {
      // The sibling was the root.
      nodes_[new_parent].child_1 = sibling;
      nodes_[new_parent].child_2 = leaf;
      nodes_[sibling].parent = new_parent;
      nodes_[leaf].parent = new_parent;
      root_ = new_parent;
    }

    // Walk back up the tree fixing heights and AABBs
    index = nodes_[leaf].parent;
    while (index != NullNode) {
      index = Balance(index);
      
      int32_t child_1 = nodes_[index].child_1;
      int32_t child_2 = nodes_[index].child_2;
      
      PHYSICS_ASSERT(child_1 != NullNode);
      PHYSICS_ASSERT(child_2 != NullNode);
      
      nodes_[index].height = 1 + math::Max(nodes_[child_1].height, nodes_[child_2].height);
      nodes_[index].aabb.Combine(nodes_[child_1].aabb, nodes_[child_2].aabb);
      
      index = nodes_[index].parent;
    }
  }
  
  void DynamicTree::RemoveLeaf(int32_t leaf) {
    if (leaf == root_) {
      root_ = NullNode;
      return;
    }
    
    int32_t parent = nodes_[leaf].parent;
    int32_t grand_parent = nodes_[parent].parent;
    int32_t sibling;
    if (nodes_[parent].child_1 == leaf) {
      sibling = nodes_[parent].child_2;
    } else {
      sibling = nodes_[parent].child_1;
    }
    
    if (grand_parent != NullNode) {
      // Destroy parent and connect sibling to grandParent.
      if (nodes_[grand_parent].child_1 == parent) {
        nodes_[grand_parent].child_1 = sibling;
      } else {
        nodes_[grand_parent].child_2 = sibling;
      }
      nodes_[sibling].parent = grand_parent;
      FreeNode(parent);
      
      // Adjust ancestor bounds.
      int32_t index = grand_parent;
      while (index != NullNode) {
        index = Balance(index);
        
        int32_t child_1 = nodes_[index].child_1;
        int32_t child_2 = nodes_[index].child_2;
        
        nodes_[index].aabb.Combine(nodes_[child_1].aabb, nodes_[child_2].aabb);
        nodes_[index].height = 1 + Max(nodes_[child_1].height, nodes_[child_2].height);
        
        index = nodes_[index].parent;
      }
    } else {
      root_ = sibling;
      nodes_[sibling].parent = NullNode;
      FreeNode(parent);
    }
    //Validate();
  }
  
  // Perform a left or right rotation if node A is imbalanced.
  // Returns the new root index.
  int32_t DynamicTree::Balance(int32_t index) {
    PHYSICS_ASSERT(index != NullNode);
    
    TreeNode* A = nodes_ + index;
    if (A->IsLeaf() || A->height < 2) {
      return index;
    }
    
    int32_t iB = A->child_1;
    int32_t iC = A->child_2;
    PHYSICS_ASSERT(0 <= iB && iB < node_capacity_);
    PHYSICS_ASSERT(0 <= iC && iC < node_capacity_);
    
    TreeNode* B = nodes_ + iB;
    TreeNode* C = nodes_ + iC;
    
    int32_t balance = C->height - B->height;
    
    // Rotate C up
    if (balance > 1) {
      int32_t iF = C->child_1;
      int32_t iG = C->child_2;
      TreeNode* F = nodes_ + iF;
      TreeNode* G = nodes_ + iG;
      PHYSICS_ASSERT(0 <= iF && iF < node_capacity_);
      PHYSICS_ASSERT(0 <= iG && iG < node_capacity_);
      
      // Swap A and C
      C->child_1 = index;
      C->parent = A->parent;
      A->parent = iC;
      
      // A's old parent should point to C
      if (C->parent != NullNode) {
        if (nodes_[C->parent].child_1 == index) {
          nodes_[C->parent].child_1 = iC;
        } else {
          PHYSICS_ASSERT(nodes_[C->parent].child_2 == index);
          nodes_[C->parent].child_2 = iC;
        }
      } else {
        root_ = iC;
      }
      
      // Rotate
      if (F->height > G->height) {
        C->child_2 = iF;
        A->child_2 = iG;
        G->parent = index;
        A->aabb.Combine(B->aabb, G->aabb);
        C->aabb.Combine(A->aabb, F->aabb);
        
        A->height = 1 + Max(B->height, G->height);
        C->height = 1 + Max(A->height, F->height);
      } else {
        C->child_2 = iG;
        A->child_2 = iF;
        F->parent = index;
        A->aabb.Combine(B->aabb, F->aabb);
        C->aabb.Combine(A->aabb, G->aabb);
        
        A->height = 1 + Max(B->height, F->height);
        C->height = 1 + Max(A->height, G->height);
      }
      
      return iC;
    }
    
    // Rotate B up
    if (balance < -1) {
      int32_t iD = B->child_1;
      int32_t iE = B->child_2;
      TreeNode* D = nodes_ + iD;
      TreeNode* E = nodes_ + iE;
      PHYSICS_ASSERT(0 <= iD && iD < node_capacity_);
      PHYSICS_ASSERT(0 <= iE && iE < node_capacity_);
      
      // Swap A and B
      B->child_1 = index;
      B->parent = A->parent;
      A->parent = iB;
      
      // A's old parent should point to B
      if (B->parent != NullNode) {
        if (nodes_[B->parent].child_1 == index) {
          nodes_[B->parent].child_1 = iB;
        } else {
          PHYSICS_ASSERT(nodes_[B->parent].child_2 == index);
          nodes_[B->parent].child_2 = iB;
        }
      } else {
        root_ = iB;
      }
      
      // Rotate
      if (D->height > E->height) {
        B->child_2 = iD;
        A->child_1 = iE;
        E->parent = index;
        A->aabb.Combine(C->aabb, E->aabb);
        B->aabb.Combine(A->aabb, D->aabb);
        
        A->height = 1 + Max(C->height, E->height);
        B->height = 1 + Max(A->height, D->height);
      } else {
        B->child_2 = iE;
        A->child_1 = iD;
        D->parent = index;
        A->aabb.Combine(C->aabb, D->aabb);
        B->aabb.Combine(A->aabb, E->aabb);
        
        A->height = 1 + Max(C->height, D->height);
        B->height = 1 + Max(A->height, E->height);
      }
      
      return iB;
    }
    return index;
  }

  int32_t DynamicTree::GetHeight() const {
    if (root_ == NullNode) {
      return 0;
    }
    return nodes_[root_].height;
  }
  
  float DynamicTree::GetAreaRatio() const {
    if (root_ == NullNode) {
      return 0.0f;
    }
    
    const TreeNode* root = nodes_ + root_;
    float rootArea = root->aabb.GetPerimeter();
    
    float totalArea = 0.0f;
    for (int32_t i = 0; i < node_capacity_; ++i) {
      const TreeNode* node = nodes_ + i;
      if (node->height < 0) {
        // Free node in pool
        continue;
      }
      
      totalArea += node->aabb.GetPerimeter();
    }
    
    return totalArea / rootArea;
  }
  
  // Compute the height of a sub-tree.
  int32_t DynamicTree::ComputeHeight(int32_t nodeId) const {
    PHYSICS_ASSERT(0 <= nodeId && nodeId < node_capacity_);
    TreeNode* node = nodes_ + nodeId;
    
    if (node->IsLeaf()) {
      return 0;
    }
    
    int32_t height1 = ComputeHeight(node->child_1);
    int32_t height2 = ComputeHeight(node->child_2);
    return 1 + Max(height1, height2);
  }
  
  int32_t DynamicTree::ComputeHeight() const {
    int32_t height = ComputeHeight(root_);
    return height;
  }
  
  void DynamicTree::ValidateStructure(int32_t index) const {
    if (index == NullNode) {
      return;
    }
    
    if (index == root_) {
      PHYSICS_ASSERT(nodes_[index].parent == NullNode);
    }
    
    const TreeNode* node = nodes_ + index;
    
    int32_t child_1 = node->child_1;
    int32_t child_2 = node->child_2;
    
    if (node->IsLeaf()) {
      PHYSICS_ASSERT(child_1 == NullNode);
      PHYSICS_ASSERT(child_2 == NullNode);
      PHYSICS_ASSERT(node->height == 0);
      return;
    }
    
    PHYSICS_ASSERT(0 <= child_1 && child_1 < node_capacity_);
    PHYSICS_ASSERT(0 <= child_2 && child_2 < node_capacity_);
    
    PHYSICS_ASSERT(nodes_[child_1].parent == index);
    PHYSICS_ASSERT(nodes_[child_2].parent == index);
    
    ValidateStructure(child_1);
    ValidateStructure(child_2);
  }
  
  void DynamicTree::ValidateMetrics(int32_t index) const {
    if (index == NullNode) {
      return;
    }
    
    const TreeNode* node = nodes_ + index;
    
    int32_t child_1 = node->child_1;
    int32_t child_2 = node->child_2;
    
    if (node->IsLeaf()) {
      PHYSICS_ASSERT(child_1 == NullNode);
      PHYSICS_ASSERT(child_2 == NullNode);
      PHYSICS_ASSERT(node->height == 0);
      return;
    }
    
    PHYSICS_ASSERT(0 <= child_1 && child_1 < node_capacity_);
    PHYSICS_ASSERT(0 <= child_2 && child_2 < node_capacity_);
    
    int32_t height1 = nodes_[child_1].height;
    int32_t height2 = nodes_[child_2].height;
    int32_t height;
    height = 1 + Max(height1, height2);
    PHYSICS_ASSERT(node->height == height);
    
    AABB aabb;
    aabb.Combine(nodes_[child_1].aabb, nodes_[child_2].aabb);
    
    PHYSICS_ASSERT(aabb.lower_bound == node->aabb.lower_bound);
    PHYSICS_ASSERT(aabb.upper_bound == node->aabb.upper_bound);
    
    ValidateMetrics(child_1);
    ValidateMetrics(child_2);
  }
  
  void DynamicTree::Validate() const {
    ValidateStructure(root_);
    ValidateMetrics(root_);
    
    int32_t freeCount = 0;
    int32_t freeIndex = free_list_;
    while (freeIndex != NullNode) {
      PHYSICS_ASSERT(0 <= freeIndex && freeIndex < node_capacity_);
      freeIndex = nodes_[freeIndex].next;
      ++freeCount;
    }
    
    PHYSICS_ASSERT(GetHeight() == ComputeHeight());
    
    PHYSICS_ASSERT(node_count_ + freeCount == node_capacity_);
  }
  
  int32_t DynamicTree::GetMaxBalance() const {
    int32_t maxBalance = 0;
    for (int32_t i = 0; i < node_capacity_; ++i) {
      const TreeNode* node = nodes_ + i;
      if (node->height <= 1) {
        continue;
      }
      
      PHYSICS_ASSERT(node->IsLeaf() == false);
      
      int32_t child_1 = node->child_1;
      int32_t child_2 = node->child_2;
      int32_t balance = Abs(nodes_[child_2].height - nodes_[child_1].height);
      maxBalance = Max(maxBalance, balance);
    }
    
    return maxBalance;
  }
  
  void DynamicTree::RebuildBottomUp() {
    int32_t* nodes = (int32_t*)utils::Alloc(node_count_ * sizeof(int32_t));
    int32_t count = 0;
    
    // Build array of leaves. Free the rest.
    for (int32_t i = 0; i < node_capacity_; ++i) {
      if (nodes_[i].height < 0) {
        // free node in pool
        continue;
      }
      
      if (nodes_[i].IsLeaf()) {
        nodes_[i].parent = NullNode;
        nodes[count] = i;
        ++count;
      } else {
        FreeNode(i);
      }
    }
    
    while (count > 1) {
      float minCost = FLT_MAX;
      int32_t iMin = -1, jMin = -1;
      for (int32_t i = 0; i < count; ++i) {
        AABB aabbi = nodes_[nodes[i]].aabb;
        
        for (int32_t j = i + 1; j < count; ++j) {
          AABB aabbj = nodes_[nodes[j]].aabb;
          AABB b;
          b.Combine(aabbi, aabbj);
          float cost = b.GetPerimeter();
          if (cost < minCost) {
            iMin = i;
            jMin = j;
            minCost = cost;
          }
        }
      }
      
      int32_t index1 = nodes[iMin];
      int32_t index2 = nodes[jMin];
      TreeNode* child_1 = nodes_ + index1;
      TreeNode* child_2 = nodes_ + index2;
      
      int32_t parentIndex = AllocateNode();
      TreeNode* parent = nodes_ + parentIndex;
      parent->child_1 = index1;
      parent->child_2 = index2;
      parent->height = 1 + Max(child_1->height, child_2->height);
      parent->aabb.Combine(child_1->aabb, child_2->aabb);
      parent->parent = NullNode;
      
      child_1->parent = parentIndex;
      child_2->parent = parentIndex;
      
      nodes[jMin] = nodes[count-1];
      nodes[iMin] = parentIndex;
      --count;
    }
    
    root_ = nodes[0];
    utils::Free(nodes);
    
    Validate();
  }
  
  void DynamicTree::ShiftOrigin(const Vec2& new_origin) {
    // Build array of leaves. Free the rest.
    for (int32_t i = 0; i < node_capacity_; ++i) {
      nodes_[i].aabb.lower_bound -= new_origin;
      nodes_[i].aabb.upper_bound -= new_origin;
    }
  }

}
