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

    IK_CORE_DEBUG("Creating Open GL Vertex Buffer with Data...");
    IK_CORE_DEBUG("  Renderer ID      | {0}", renderer_id_);
    IK_CORE_DEBUG("  Size             | {0} Bytes ({1} KB {2} MB)",
                  size_, size_ / 1000, size_ / 1000000);
    
    uint32_t total_size = RendererStatistics::Get().vertex_buffer_size;
    IK_CORE_DEBUG("  Total Size Used  | {0} Bytes ({1} KB {2} MB)",
                  total_size, total_size / 1000, total_size / 1000000);
  }
  
  OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    RendererStatistics::Get().vertex_buffer_size -= size_;
    
    IK_CORE_WARN("Destroying Open GL Vertex Buffer !!!");
    IK_CORE_WARN("  Renderer ID | {0}", renderer_id_);
    IK_CORE_WARN("  Size        | {0} Bytes ({1} KB, {2} MB)",
                 size_, size_ / 1000, size_ / 1000000);
    
    IDManager::RemoveBufferId(renderer_id_);
  }

  // --------------------------------------------------------------------------
  // Index Buffer
  // --------------------------------------------------------------------------
  OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t size)
  : size_(size), count_(size_ / sizeof(uint32_t)) {
    IDManager::GetBufferId(renderer_id_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_, data, GL_STATIC_DRAW);
    
    RendererStatistics::Get().index_buffer_size += size_;
    
    IK_CORE_DEBUG("Creating Open GL Index Buffer ...");
    IK_CORE_DEBUG("  Renderer ID       | {0}", renderer_id_);
    IK_CORE_DEBUG("  Number of Indices | {0}", count_);
    IK_CORE_DEBUG("  Size              | {0} Bytes ({1} KB, {2} MB)",
                  size_, size_ / 1000, size_ / 1000000);
    uint32_t total_size = RendererStatistics::Get().index_buffer_size;
    IK_CORE_DEBUG("  Total Size Used   | {0} Bytes ({1} KB {2} MB)",
                  total_size, total_size / 1000, total_size / 1000000);
  }
  
  OpenGLIndexBuffer::~OpenGLIndexBuffer() noexcept {
    RendererStatistics::Get().index_buffer_size -= size_;
    
    IK_CORE_WARN("Destroying Open GL Index Buffer !!!");
    IK_CORE_WARN("  Renderer ID       | {0}", renderer_id_);
    IK_CORE_WARN("  Number of Indices | {0}", count_);
    IK_CORE_WARN("  Size              | {0} Bytes ({1} KB, {2} MB)",
                 size_, size_ / 1000, size_ / 1000000);

    IDManager::RemoveBufferId(renderer_id_);
  }

}
