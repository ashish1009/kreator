//
//  aabb_renderer.hpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#pragma once

#include "physics/aabb.hpp"

namespace ikan {
  
  class AABBRenderer {
  public:
    /// This function initialize the box 3d Renderer
    static void Init();
    /// This function Shutdown the box 3d Renderer
    static void Shutdown();
    
    /// This function begins the renderer for aabb
    /// - Parameter viewProj: camera view projection
    static void BeginRenderer(const glm::mat4& view_projection);
    /// This function ends the renderer for aabb
    static void EndRenderer();
    
    /// This function render the Box 3d
    /// - Parameter aabb: aabb of the box
    static void Draw(const AABB& aabb);

    MAKE_PURE_STATIC(AABBRenderer);
  };
  
} // namesapce editor

