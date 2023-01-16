//
//  dynamic_tree.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "dynamic_tree.hpp"

namespace physics {
  
  DynamicTree::DynamicTree() {
    root_ = NullNode;
    
    node_capacity_ = 16;
    node_count_ = 0;
    nodes_ = (TreeNode*)AllocMem(node_capacity_ * sizeof(TreeNode));
    memset(nodes_, 0, node_capacity_ * sizeof(TreeNode));
    
    // Build a linked list for the FreeMem list.
    for (int32_t i = 0; i < node_capacity_ - 1; ++i) {
      nodes_[i].next = i + 1;
      nodes_[i].height = -1;
    }
    nodes_[node_capacity_-1].next = NullNode;
    nodes_[node_capacity_-1].height = -1;
    free_list_ = 0;
    
    insertion_count_ = 0;
  }
  
  DynamicTree::~DynamicTree() {
    // This frees the entire tree in one shot.
    FreeMem(nodes_);
  }
  
  // Allocate a node from the pool. Grow the pool if necessary.
  int32_t DynamicTree::AllocateNode() {
    // Expand the node pool as needed.
    if (free_list_ == NullNode) {
      IK_ASSERT(node_count_ == node_capacity_);
      
      // The FreeMem list is empty. Rebuild a bigger pool.
      TreeNode* oldNodes = nodes_;
      node_capacity_ *= 2;
      nodes_ = (TreeNode*)AllocMem(node_capacity_ * sizeof(TreeNode));
      memcpy(nodes_, oldNodes, node_count_ * sizeof(TreeNode));
      FreeMem(oldNodes);
      
      // Build a linked list for the FreeMem list. The parent
      // pointer becomes the "next" pointer.
      for (int32_t i = node_count_; i < node_capacity_ - 1; ++i)
      {
        nodes_[i].next = i + 1;
        nodes_[i].height = -1;
      }
      nodes_[node_capacity_-1].next = NullNode;
      nodes_[node_capacity_-1].height = -1;
      free_list_ = node_count_;
    }
    
    // Peel a node off the FreeMem list.
    int32_t nodeId = free_list_;
    free_list_ = nodes_[nodeId].next;
    nodes_[nodeId].parent = NullNode;
    nodes_[nodeId].child1 = NullNode;
    nodes_[nodeId].child2 = NullNode;
    nodes_[nodeId].height = 0;
    nodes_[nodeId].userData = nullptr;
    nodes_[nodeId].moved = false;
    ++node_count_;
    return nodeId;
  }
  
  // Return a node to the pool.
  void DynamicTree::FreeNode(int32_t nodeId) {
    IK_ASSERT(0 <= nodeId && nodeId < node_capacity_);
    IK_ASSERT(0 < node_count_);
    nodes_[nodeId].next = free_list_;
    nodes_[nodeId].height = -1;
    free_list_ = nodeId;
    --node_count_;
  }
  
  // Create a proxy in the tree as a leaf node. We return the index
  // of the node instead of a pointer so that we can grow
  // the node pool.
  int32_t DynamicTree::CreateProxy(const AABB& aabb, void* userData) {
    int32_t proxyId = AllocateNode();
    
    // Fatten the aabb.
    Vec2 r(AabbExtension, AabbExtension);
    nodes_[proxyId].aabb.lowerBound = aabb.lowerBound - r;
    nodes_[proxyId].aabb.upperBound = aabb.upperBound + r;
    nodes_[proxyId].userData = userData;
    nodes_[proxyId].height = 0;
    nodes_[proxyId].moved = true;
    
    InsertLeaf(proxyId);
    
    return proxyId;
  }
  
  void DynamicTree::DestroyProxy(int32_t proxyId) {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    IK_ASSERT(nodes_[proxyId].IsLeaf());
    
    RemoveLeaf(proxyId);
    FreeNode(proxyId);
  }
  
  bool DynamicTree::MoveProxy(int32_t proxyId, const AABB& aabb, const Vec2& displacement) {
    IK_ASSERT(0 <= proxyId && proxyId < node_capacity_);
    
    IK_ASSERT(nodes_[proxyId].IsLeaf());
    
    // Extend AABB
    AABB fatAABB;
    Vec2 r(AabbExtension, AabbExtension);
    fatAABB.lowerBound = aabb.lowerBound - r;
    fatAABB.upperBound = aabb.upperBound + r;
    
    // Predict AABB movement
    Vec2 d = AabbMultiplier * displacement;
    
    if (d.x < 0.0f) {
      fatAABB.lowerBound.x += d.x;
    }
    else {
      fatAABB.upperBound.x += d.x;
    }
    
    if (d.y < 0.0f) {
      fatAABB.lowerBound.y += d.y;
    }
    else {
      fatAABB.upperBound.y += d.y;
    }
    
    const AABB& treeAABB = nodes_[proxyId].aabb;
    if (treeAABB.Contains(aabb)) {
      // The tree AABB still contains the object, but it might be too large.
      // Perhaps the object was moving fast but has since gone to sleep.
      // The huge AABB is larger than the new fat AABB.
      AABB hugeAABB;
      hugeAABB.lowerBound = fatAABB.lowerBound - 4.0f * r;
      hugeAABB.upperBound = fatAABB.upperBound + 4.0f * r;
      
      if (hugeAABB.Contains(treeAABB))
      {
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
  
  void DynamicTree::InsertLeaf(int32_t leaf) {
    ++insertion_count_;
    
    if ( root_ == NullNode) {
       root_ = leaf;
      nodes_[ root_].parent = NullNode;
      return;
    }
    
    // Find the best sibling for this node
    AABB leafAABB = nodes_[leaf].aabb;
    int32_t index =  root_;
    while (nodes_[index].IsLeaf() == false) {
      int32_t child1 = nodes_[index].child1;
      int32_t child2 = nodes_[index].child2;
      
      float area = nodes_[index].aabb.GetPerimeter();
      
      AABB combinedAABB;
      combinedAABB.Combine(nodes_[index].aabb, leafAABB);
      float combinedArea = combinedAABB.GetPerimeter();
      
      // Cost of creating a new parent for this node and the new leaf
      float cost = 2.0f * combinedArea;
      
      // Minimum cost of pushing the leaf further down the tree
      float inheritanceCost = 2.0f * (combinedArea - area);
      
      // Cost of descending into child1
      float cost1;
      if (nodes_[child1].IsLeaf())
      {
        AABB aabb;
        aabb.Combine(leafAABB, nodes_[child1].aabb);
        cost1 = aabb.GetPerimeter() + inheritanceCost;
      }
      else
      {
        AABB aabb;
        aabb.Combine(leafAABB, nodes_[child1].aabb);
        float oldArea = nodes_[child1].aabb.GetPerimeter();
        float newArea = aabb.GetPerimeter();
        cost1 = (newArea - oldArea) + inheritanceCost;
      }
      
      // Cost of descending into child2
      float cost2;
      if (nodes_[child2].IsLeaf())
      {
        AABB aabb;
        aabb.Combine(leafAABB, nodes_[child2].aabb);
        cost2 = aabb.GetPerimeter() + inheritanceCost;
      }
      else
      {
        AABB aabb;
        aabb.Combine(leafAABB, nodes_[child2].aabb);
        float oldArea = nodes_[child2].aabb.GetPerimeter();
        float newArea = aabb.GetPerimeter();
        cost2 = newArea - oldArea + inheritanceCost;
      }
      
      // Descend according to the minimum cost.
      if (cost < cost1 && cost < cost2)
      {
        break;
      }
      
      // Descend
      if (cost1 < cost2)
      {
        index = child1;
      }
      else
      {
        index = child2;
      }
    }
    
    int32_t sibling = index;
    
    // Create a new parent.
    int32_t oldParent = nodes_[sibling].parent;
    int32_t newParent = AllocateNode();
    nodes_[newParent].parent = oldParent;
    nodes_[newParent].userData = nullptr;
    nodes_[newParent].aabb.Combine(leafAABB, nodes_[sibling].aabb);
    nodes_[newParent].height = nodes_[sibling].height + 1;
    
    if (oldParent != NullNode) {
      // The sibling was not the root.
      if (nodes_[oldParent].child1 == sibling)
      {
        nodes_[oldParent].child1 = newParent;
      }
      else
      {
        nodes_[oldParent].child2 = newParent;
      }
      
      nodes_[newParent].child1 = sibling;
      nodes_[newParent].child2 = leaf;
      nodes_[sibling].parent = newParent;
      nodes_[leaf].parent = newParent;
    }
    else {
      // The sibling was the root.
      nodes_[newParent].child1 = sibling;
      nodes_[newParent].child2 = leaf;
      nodes_[sibling].parent = newParent;
      nodes_[leaf].parent = newParent;
       root_ = newParent;
    }
    
    // Walk back up the tree fixing heights and AABBs
    index = nodes_[leaf].parent;
    while (index != NullNode) {
      index = Balance(index);
      
      int32_t child1 = nodes_[index].child1;
      int32_t child2 = nodes_[index].child2;
      
      IK_ASSERT(child1 != NullNode);
      IK_ASSERT(child2 != NullNode);
      
      nodes_[index].height = 1 + Max(nodes_[child1].height, nodes_[child2].height);
      nodes_[index].aabb.Combine(nodes_[child1].aabb, nodes_[child2].aabb);
      
      index = nodes_[index].parent;
    }
    
    //Validate();
  }
  
  void DynamicTree::RemoveLeaf(int32_t leaf) {
    if (leaf ==  root_) {
       root_ = NullNode;
      return;
    }
    
    int32_t parent = nodes_[leaf].parent;
    int32_t grandParent = nodes_[parent].parent;
    int32_t sibling;
    if (nodes_[parent].child1 == leaf) {
      sibling = nodes_[parent].child2;
    }
    else {
      sibling = nodes_[parent].child1;
    }
    
    if (grandParent != NullNode) {
      // Destroy parent and connect sibling to grandParent.
      if (nodes_[grandParent].child1 == parent)
      {
        nodes_[grandParent].child1 = sibling;
      }
      else
      {
        nodes_[grandParent].child2 = sibling;
      }
      nodes_[sibling].parent = grandParent;
      FreeNode(parent);
      
      // Adjust ancestor bounds.
      int32_t index = grandParent;
      while (index != NullNode)
      {
        index = Balance(index);
        
        int32_t child1 = nodes_[index].child1;
        int32_t child2 = nodes_[index].child2;
        
        nodes_[index].aabb.Combine(nodes_[child1].aabb, nodes_[child2].aabb);
        nodes_[index].height = 1 + Max(nodes_[child1].height, nodes_[child2].height);
        
        index = nodes_[index].parent;
      }
    }
    else {
       root_ = sibling;
      nodes_[sibling].parent = NullNode;
      FreeNode(parent);
    }
    
    //Validate();
  }
  
  // Perform a left or right rotation if node A is imbalanced.
  // Returns the new root index.
  int32_t DynamicTree::Balance(int32_t iA) {
    IK_ASSERT(iA != NullNode);
    
    TreeNode* A = nodes_ + iA;
    if (A->IsLeaf() || A->height < 2) {
      return iA;
    }
    
    int32_t iB = A->child1;
    int32_t iC = A->child2;
    IK_ASSERT(0 <= iB && iB < node_capacity_);
    IK_ASSERT(0 <= iC && iC < node_capacity_);
    
    TreeNode* B = nodes_ + iB;
    TreeNode* C = nodes_ + iC;
    
    int32_t balance = C->height - B->height;
    
    // Rotate C up
    if (balance > 1) {
      int32_t iF = C->child1;
      int32_t iG = C->child2;
      TreeNode* F = nodes_ + iF;
      TreeNode* G = nodes_ + iG;
      IK_ASSERT(0 <= iF && iF < node_capacity_);
      IK_ASSERT(0 <= iG && iG < node_capacity_);
      
      // Swap A and C
      C->child1 = iA;
      C->parent = A->parent;
      A->parent = iC;
      
      // A's old parent should point to C
      if (C->parent != NullNode)
      {
        if (nodes_[C->parent].child1 == iA)
        {
          nodes_[C->parent].child1 = iC;
        }
        else
        {
          IK_ASSERT(nodes_[C->parent].child2 == iA);
          nodes_[C->parent].child2 = iC;
        }
      }
      else
      {
         root_ = iC;
      }
      
      // Rotate
      if (F->height > G->height)
      {
        C->child2 = iF;
        A->child2 = iG;
        G->parent = iA;
        A->aabb.Combine(B->aabb, G->aabb);
        C->aabb.Combine(A->aabb, F->aabb);
        
        A->height = 1 + Max(B->height, G->height);
        C->height = 1 + Max(A->height, F->height);
      }
      else
      {
        C->child2 = iG;
        A->child2 = iF;
        F->parent = iA;
        A->aabb.Combine(B->aabb, F->aabb);
        C->aabb.Combine(A->aabb, G->aabb);
        
        A->height = 1 + Max(B->height, F->height);
        C->height = 1 + Max(A->height, G->height);
      }
      
      return iC;
    }
    
    // Rotate B up
    if (balance < -1) {
      int32_t iD = B->child1;
      int32_t iE = B->child2;
      TreeNode* D = nodes_ + iD;
      TreeNode* E = nodes_ + iE;
      IK_ASSERT(0 <= iD && iD < node_capacity_);
      IK_ASSERT(0 <= iE && iE < node_capacity_);
      
      // Swap A and B
      B->child1 = iA;
      B->parent = A->parent;
      A->parent = iB;
      
      // A's old parent should point to B
      if (B->parent != NullNode)
      {
        if (nodes_[B->parent].child1 == iA)
        {
          nodes_[B->parent].child1 = iB;
        }
        else
        {
          IK_ASSERT(nodes_[B->parent].child2 == iA);
          nodes_[B->parent].child2 = iB;
        }
      }
      else
      {
         root_ = iB;
      }
      
      // Rotate
      if (D->height > E->height)
      {
        B->child2 = iD;
        A->child1 = iE;
        E->parent = iA;
        A->aabb.Combine(C->aabb, E->aabb);
        B->aabb.Combine(A->aabb, D->aabb);
        
        A->height = 1 + Max(C->height, E->height);
        B->height = 1 + Max(A->height, D->height);
      }
      else
      {
        B->child2 = iE;
        A->child1 = iD;
        D->parent = iA;
        A->aabb.Combine(C->aabb, D->aabb);
        B->aabb.Combine(A->aabb, E->aabb);
        
        A->height = 1 + Max(C->height, D->height);
        B->height = 1 + Max(A->height, E->height);
      }
      
      return iB;
    }
    
    return iA;
  }
  
  int32_t DynamicTree::GetHeight() const {
    if ( root_ == NullNode) {
      return 0;
    }
    
    return nodes_[ root_].height;
  }
  
  //
  float DynamicTree::GetAreaRatio() const {
    if ( root_ == NullNode) {
      return 0.0f;
    }
    
    const TreeNode* root = nodes_ +  root_;
    float rootArea = root->aabb.GetPerimeter();
    
    float totalArea = 0.0f;
    for (int32_t i = 0; i < node_capacity_; ++i) {
      const TreeNode* node = nodes_ + i;
      if (node->height < 0)
      {
        // FreeMem node in pool
        continue;
      }
      
      totalArea += node->aabb.GetPerimeter();
    }
    
    return totalArea / rootArea;
  }
  
  // Compute the height of a sub-tree.
  int32_t DynamicTree::ComputeHeight(int32_t nodeId) const {
    IK_ASSERT(0 <= nodeId && nodeId < node_capacity_);
    TreeNode* node = nodes_ + nodeId;
    
    if (node->IsLeaf()) {
      return 0;
    }
    
    int32_t height1 = ComputeHeight(node->child1);
    int32_t height2 = ComputeHeight(node->child2);
    return 1 + Max(height1, height2);
  }
  
  int32_t DynamicTree::ComputeHeight() const {
    int32_t height = ComputeHeight( root_);
    return height;
  }
  
  void DynamicTree::ValidateStructure(int32_t index) const {
    if (index == NullNode) {
      return;
    }
    
    if (index ==  root_) {
      IK_ASSERT(nodes_[index].parent == NullNode);
    }
    
    const TreeNode* node = nodes_ + index;
    
    int32_t child1 = node->child1;
    int32_t child2 = node->child2;
    
    if (node->IsLeaf()) {
      IK_ASSERT(child1 == NullNode);
      IK_ASSERT(child2 == NullNode);
      IK_ASSERT(node->height == 0);
      return;
    }
    
    IK_ASSERT(0 <= child1 && child1 < node_capacity_);
    IK_ASSERT(0 <= child2 && child2 < node_capacity_);
    
    IK_ASSERT(nodes_[child1].parent == index);
    IK_ASSERT(nodes_[child2].parent == index);
    
    ValidateStructure(child1);
    ValidateStructure(child2);
  }
  
  void DynamicTree::ValidateMetrics(int32_t index) const {
    if (index == NullNode) {
      return;
    }
    
    const TreeNode* node = nodes_ + index;
    
    int32_t child1 = node->child1;
    int32_t child2 = node->child2;
    
    if (node->IsLeaf()) {
      IK_ASSERT(child1 == NullNode);
      IK_ASSERT(child2 == NullNode);
      IK_ASSERT(node->height == 0);
      return;
    }
    
    IK_ASSERT(0 <= child1 && child1 < node_capacity_);
    IK_ASSERT(0 <= child2 && child2 < node_capacity_);
    
    int32_t height1 = nodes_[child1].height;
    int32_t height2 = nodes_[child2].height;
    int32_t height;
    height = 1 + Max(height1, height2);
    IK_ASSERT(node->height == height);
    
    AABB aabb;
    aabb.Combine(nodes_[child1].aabb, nodes_[child2].aabb);
    
    IK_ASSERT(aabb.lowerBound == node->aabb.lowerBound);
    IK_ASSERT(aabb.upperBound == node->aabb.upperBound);
    
    ValidateMetrics(child1);
    ValidateMetrics(child2);
  }
  
  void DynamicTree::Validate() const {
#if defined(DEBUG)
    ValidateStructure( root_);
    ValidateMetrics( root_);
    
    int32_t freeCount = 0;
    int32_t freeIndex = free_list_;
    while (freeIndex != NullNode) {
      IK_ASSERT(0 <= freeIndex && freeIndex < node_capacity_);
      freeIndex = nodes_[freeIndex].next;
      ++freeCount;
    }
    
    IK_ASSERT(GetHeight() == ComputeHeight());
    
    IK_ASSERT(node_count_ + freeCount == node_capacity_);
#endif
  }
  
  int32_t DynamicTree::GetMaxBalance() const {
    int32_t maxBalance = 0;
    for (int32_t i = 0; i < node_capacity_; ++i) {
      const TreeNode* node = nodes_ + i;
      if (node->height <= 1)
      {
        continue;
      }
      
      IK_ASSERT(node->IsLeaf() == false);
      
      int32_t child1 = node->child1;
      int32_t child2 = node->child2;
      int32_t balance = Abs(nodes_[child2].height - nodes_[child1].height);
      maxBalance = Max(maxBalance, balance);
    }
    
    return maxBalance;
  }
  
  void DynamicTree::RebuildBottomUp() {
    int32_t* nodes = (int32_t*)AllocMem(node_count_ * sizeof(int32_t));
    int32_t count = 0;
    
    // Build array of leaves. FreeMem the rest.
    for (int32_t i = 0; i < node_capacity_; ++i) {
      if (nodes_[i].height < 0)
      {
        // FreeMem node in pool
        continue;
      }
      
      if (nodes_[i].IsLeaf())
      {
        nodes_[i].parent = NullNode;
        nodes[count] = i;
        ++count;
      }
      else
      {
        FreeNode(i);
      }
    }
    
    while (count > 1) {
      float minCost = FLT_MAX;
      int32_t iMin = -1, jMin = -1;
      for (int32_t i = 0; i < count; ++i)
      {
        AABB aabbi = nodes_[nodes[i]].aabb;
        
        for (int32_t j = i + 1; j < count; ++j)
        {
          AABB aabbj = nodes_[nodes[j]].aabb;
          AABB b;
          b.Combine(aabbi, aabbj);
          float cost = b.GetPerimeter();
          if (cost < minCost)
          {
            iMin = i;
            jMin = j;
            minCost = cost;
          }
        }
      }
      
      int32_t index1 = nodes[iMin];
      int32_t index2 = nodes[jMin];
      TreeNode* child1 = nodes_ + index1;
      TreeNode* child2 = nodes_ + index2;
      
      int32_t parentIndex = AllocateNode();
      TreeNode* parent = nodes_ + parentIndex;
      parent->child1 = index1;
      parent->child2 = index2;
      parent->height = 1 + Max(child1->height, child2->height);
      parent->aabb.Combine(child1->aabb, child2->aabb);
      parent->parent = NullNode;
      
      child1->parent = parentIndex;
      child2->parent = parentIndex;
      
      nodes[jMin] = nodes[count-1];
      nodes[iMin] = parentIndex;
      --count;
    }
    
     root_ = nodes[0];
    FreeMem(nodes);
    
    Validate();
  }
  
  void DynamicTree::ShiftOrigin(const Vec2& newOrigin) {
    // Build array of leaves. FreeMem the rest.
    for (int32_t i = 0; i < node_capacity_; ++i) {
      nodes_[i].aabb.lowerBound -= newOrigin;
      nodes_[i].aabb.upperBound -= newOrigin;
    }
  }

  
}
