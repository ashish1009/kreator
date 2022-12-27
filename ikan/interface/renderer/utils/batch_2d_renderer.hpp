//
//  batch_2d_renderer.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

// This file includes batch renderer that is resposible for
// - Initialize all Renderer buffer and store them for rendering 1 Batch in single
// draw call
// - User friendly API to render any 2D Shape (For now Quad and Cirlce)

namespace ikan {
  
  class Texture;
  
  class BatchRenderer {
  public:
    /// This function initialzes the Batch renderer. Create All buffers needed to store Data (Both
    /// Renderer and CPU)
    static void Init();
    /// This functio dhutdown the batch renderer and destoy all the buffer reserved on initialize
    static void Shutdown();
    
    /// This function begins the Batch for 2D Rendere (to be called each frame)
    /// - Parameters:
    ///   - camera_view_projection_matrix: Camera View projection Matrix
    ///   - camera_view_matrix: Camera View Matrix
    static void BeginBatch(const glm::mat4& camera_view_projection_matrix,
                           const glm::mat4& camera_view_matrix);
    /// This function Ends the current batch by rendering all the vertex
    static void EndBatch();

  private:
    // Member Methods
    /// This funcition initialize the quad renderer data
    /// - Parameter max_quads: max quad to be renderered in single batch
    static void InitQuadData(uint32_t max_quads = 50);
    /// This funcition initialize the circle renderer data
    /// - Parameter max_circles: max circle to be renderered in single batch
    static void InitCircleData(uint32_t max_circles = 50);
    /// This funcition initialize the lines renderer data
    /// - Parameter max_lines: max circle to be renderered in single batch
    static void InitLineData(uint32_t max_lines = 100);
  };
  
}
