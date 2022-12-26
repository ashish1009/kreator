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
  
  /// This class is the implementation of Open GL Renderer Vertex Buffer, to store the vertices of the objects.
  class OpenGLVertexBuffer : public VertexBuffer {
  public:
    // ---------------------------------
    // Constructors and Destructor
    // ---------------------------------
    /// This constructor creates Buffer to store the vertices of the data. This is static data buffer which
    /// takes the data pointer in the argument
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    OpenGLVertexBuffer(void* data, uint32_t size);
    /// This destructor destroy the Renderer Index Buffer
    ~OpenGLVertexBuffer();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLVertexBuffer);
    
    // --------------
    // Fundamentals
    // --------------
    /// This function uptate the Buffer layeout value in Vertex Buffer
    /// - Parameter layout: new Buffer layout
    void AddLayout(const BufferLayout& layout) override;

  public:
    RendererID renderer_id_ = 0;
    uint32_t size_ = 0;
    BufferLayout layout_;
  };
  
  /// This class is the implementation of Open GL of Renderer Index Buffer, to store the indices of the objects.
  class OpenGLIndexBuffer : public IndexBuffer {
  public:
    // ---------------------------------
    // Constructors and Destructor
    // ---------------------------------
    /// This constructor creates the Buffer to store the indices. This api takes the date buffer and size of buffer
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of index buffer
    OpenGLIndexBuffer(void* data, uint32_t size);
    /// This destructor destroy the Renderer Index Buffer
    ~OpenGLIndexBuffer() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLIndexBuffer);
    
  private:
    RendererID renderer_id_ = 0;
    uint32_t size_ = 0;
    uint32_t count_ = 0;
  };
  
}
