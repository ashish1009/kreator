//
//  open_gl_renderer_buffer.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the implementaiton of Open GL Renderer Buffers

#include "renderer/graphics/renderer_buffer.hpp"

namespace ikan {
  
  /// This class is the implementation of Open GL Renderer Vertex Buffer,
  /// to store the vertices of the objects.
  class OpenGLVertexBuffer : public VertexBuffer {
  public:
    /// This constructor creates Buffer to store the vertices of the data.
    /// This is static data buffer which takes the data pointer in the argument
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    OpenGLVertexBuffer(void* data, uint32_t size);
    /// This destructor destroy the Renderer Index Buffer
    ~OpenGLVertexBuffer();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLVertexBuffer);

  public:
    RendererID renderer_id_ = 0;
    uint32_t size_ = 0;
  };
  
}
