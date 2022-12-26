//
//  renderer_buffer.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "renderer_buffer.hpp"
#include "platform/open_gl/open_gl_renderer_buffer.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Vertex Buffer
  // --------------------------------------------------------------------------
  std::shared_ptr<VertexBuffer> VertexBuffer::Create(void *data, uint32_t size) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLVertexBuffer>(data, size);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Index Buffer
  // --------------------------------------------------------------------------
  std::shared_ptr<IndexBuffer> IndexBuffer::CreateWithCount(void* data, uint32_t count) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl : {
        uint32_t index_buffer_size = count * sizeof(uint32_t);
        return std::make_shared<OpenGLIndexBuffer>(data, index_buffer_size);
      }
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }

}
