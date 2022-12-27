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
    /// This constructor creates the Buffer to store the vertices of the data. This is a dynamic data
    /// buffer, takes only size of buffer
    /// - Parameter size: size of data
    OpenGLVertexBuffer(uint32_t size);
    /// This destructor destroy the Renderer Index Buffer
    ~OpenGLVertexBuffer();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLVertexBuffer);
    
    // --------------
    // Fundamentals
    // --------------
    /// This function uptate the Buffer layeout value in Vertex Buffer
    /// - Parameter layout: new Buffer layout
    void AddLayout(const BufferLayout& layout) override;
    /// This function update the date in buffer dynamically
    /// - Parameters:
    ///   - data: Data pointer to be stored in GPU
    ///   - size: size of data
    void SetData(void* data, uint32_t size) override;
    /// This function binds the Vertex Buffer before rendering
    void Bind() const override;
    /// This function unbinds the Vertex Buffer after rendering
    void Unbind() const override;

    // -----------
    // Getters
    // -----------
    /// This function returns the renderer ID of Vertex Buffer
    RendererID GetRendererID() const override;
    /// This function returns the Buffer layout stored in Vertex Buffer
    const BufferLayout& GetLayout() const override;
    /// This function returns the size of Vertex Buffer in GPU
    uint32_t GetSize() const override;

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
    
    // --------------
    // Fundamentals
    // --------------
    /// This function binds the Vertex Buffer before rendering
    void Bind() const override;
    /// This function unbinds the Vertex Buffer after rendering
    void Unbind() const override;

    // -----------
    // Getters
    // -----------
    /// This function returns the Number of Indices used by this Index Buffer
    uint32_t GetCount() const override;
    /// This function returns the size of Index Buffer in GPU
    uint32_t GetSize() const override;
    /// This function return the renderer ID of Index Buffer
    RendererID GetRendererID() const override;
    
  private:
    RendererID renderer_id_ = 0;
    uint32_t size_ = 0;
    uint32_t count_ = 0;
  };
  
}
