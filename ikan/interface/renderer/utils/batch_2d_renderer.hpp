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
    
  private:
    // Member Methods
    /// This funcition initialize the quad renderer data
    /// - Parameter max_quads: max quad to be renderered in single batch
    static void InitQuadData(uint32_t max_quads);
    /// This funcition initialize the circle renderer data
    /// - Parameter mac_circles: max circle to be renderered in single batch
    static void InitCircleData(uint32_t mac_circles);
  };
  
}
