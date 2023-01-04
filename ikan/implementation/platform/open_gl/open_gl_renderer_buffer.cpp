//
//  open_gl_renderer_buffer.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "open_gl_renderer_buffer.hpp"
#include "renderer/utils/renderer_stats.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"
#include <glad/glad.h>

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Vertex Buffer
  // --------------------------------------------------------------------------
  OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size) : size_(size) {
    IDManager::GetBufferId(renderer_id_);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
    glBufferData(GL_ARRAY_BUFFER, size_, data, GL_STATIC_DRAW);
    
    RendererStatistics::Get().vertex_buffer_size += size_;

    IK_CORE_DEBUG(LogModule::VertexBuffer, "Creating Open GL Vertex Buffer with Data...");
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Renderer ID      | {0}", renderer_id_);
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Size             | {0} Bytes ({1} KB {2} MB)",
                  size_, size_ / 1000, size_ / 1000000);
    
    uint32_t total_size = RendererStatistics::Get().vertex_buffer_size;
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Total Size Used  | {0} Bytes ({1} KB {2} MB)",
                  total_size, total_size / 1000, total_size / 1000000);
  }
  
  OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) : size_(size) {
    IDManager::GetBufferId(renderer_id_);

    glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
    glBufferData(GL_ARRAY_BUFFER, size_, nullptr, GL_DYNAMIC_DRAW);
    
    RendererStatistics::Get().vertex_buffer_size += size_;
    
    IK_CORE_DEBUG(LogModule::VertexBuffer, "Creating Open GL Vertex Buffer withoud Data ...");
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Renderer ID      | {0}", renderer_id_);
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Size             | {0} Bytes ({1} KB, {2} MB)",
                  size_, size_ / 1000, size_ / 1000000);
    uint32_t total_size = RendererStatistics::Get().vertex_buffer_size;
    IK_CORE_DEBUG(LogModule::VertexBuffer, "  Total Size Used  | {0} Bytes ({1} KB {2} MB)",
                  total_size, total_size / 1000, total_size / 1000000);
    
  }

  
  OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    RendererStatistics::Get().vertex_buffer_size -= size_;
    
    IK_CORE_WARN(LogModule::VertexBuffer, "Destroying Open GL Vertex Buffer !!!");
    IK_CORE_WARN(LogModule::VertexBuffer, "  Renderer ID | {0}", renderer_id_);
    IK_CORE_WARN(LogModule::VertexBuffer, "  Size        | {0} Bytes ({1} KB, {2} MB)",
                 size_, size_ / 1000, size_ / 1000000);
    
    IDManager::RemoveBufferId(renderer_id_);
  }
  
  void OpenGLVertexBuffer::SetData(void* data, uint32_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
  }
  
  void OpenGLVertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
  }
  
  void OpenGLVertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  
  void OpenGLVertexBuffer::AddLayout(const BufferLayout& layout) { layout_ = layout; }
  const BufferLayout& OpenGLVertexBuffer::GetLayout() const { return layout_; }
  RendererID OpenGLVertexBuffer::GetRendererID() const { return renderer_id_; }
  uint32_t OpenGLVertexBuffer::GetSize() const { return size_; }

  // --------------------------------------------------------------------------
  // Index Buffer
  // --------------------------------------------------------------------------
  OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t size)
  : size_(size), count_(size_ / sizeof(uint32_t)) {
    IDManager::GetBufferId(renderer_id_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_, data, GL_STATIC_DRAW);
    
    RendererStatistics::Get().index_buffer_size += size_;
    
    IK_CORE_DEBUG(LogModule::IndexBuffer, "Creating Open GL Index Buffer ...");
    IK_CORE_DEBUG(LogModule::IndexBuffer, "  Renderer ID       | {0}", renderer_id_);
    IK_CORE_DEBUG(LogModule::IndexBuffer, "  Number of Indices | {0}", count_);
    IK_CORE_DEBUG(LogModule::IndexBuffer, "  Size              | {0} Bytes ({1} KB, {2} MB)",
                  size_, size_ / 1000, size_ / 1000000);
    uint32_t total_size = RendererStatistics::Get().index_buffer_size;
    IK_CORE_DEBUG(LogModule::IndexBuffer, "  Total Size Used   | {0} Bytes ({1} KB {2} MB)",
                  total_size, total_size / 1000, total_size / 1000000);
  }
  
  OpenGLIndexBuffer::~OpenGLIndexBuffer() noexcept {
    RendererStatistics::Get().index_buffer_size -= size_;
    
    IK_CORE_WARN(LogModule::IndexBuffer, "Destroying Open GL Index Buffer !!!");
    IK_CORE_WARN(LogModule::IndexBuffer, "  Renderer ID       | {0}", renderer_id_);
    IK_CORE_WARN(LogModule::IndexBuffer, "  Number of Indices | {0}", count_);
    IK_CORE_WARN(LogModule::IndexBuffer, "  Size              | {0} Bytes ({1} KB, {2} MB)",
                 size_, size_ / 1000, size_ / 1000000);

    IDManager::RemoveBufferId(renderer_id_);
  }
  
  void OpenGLIndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
  }
  
  void OpenGLIndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  
  RendererID OpenGLIndexBuffer::GetRendererID() const { return renderer_id_;}
  uint32_t OpenGLIndexBuffer::GetCount() const { return count_; }
  uint32_t OpenGLIndexBuffer::GetSize() const { return size_; }

}
