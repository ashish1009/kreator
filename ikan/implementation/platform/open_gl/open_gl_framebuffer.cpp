//
//  open_gl_framebuffer.cpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#include "open_gl_framebuffer.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"
#include <glad/glad.h>

namespace ikan {
  
  namespace frame_buffer_utils {
    /// This function checks is the specification format is depth or not
    /// - Parameter format: Format type
    static bool IsDepthFormat(FrameBuffer::Attachments::TextureFormat format) {
      switch (format) {
        case FrameBuffer::Attachments::TextureFormat::RGBA8:
          return false;
          
        case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
          return true;
          
        case FrameBuffer::Attachments::TextureFormat::None:
        default:
          IK_CORE_ASSERT(false, "invalid format");
      }
    }

  }
  
  // --------------------------------------------------------------------------
  // Open GL FrameBuffer API
  // --------------------------------------------------------------------------
  OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBuffer::Specification& specs)
  : specification_(specs) {
    IK_CORE_DEBUG("Creating Open GL Framebuffer ...");
    
    for (auto attchment_format : specification_.attachments.texture_formats) {
      if (frame_buffer_utils::IsDepthFormat(attchment_format))
        depth_specification_ = attchment_format;
      else
        color_specifications_.emplace_back(attchment_format);
    }
  }

  OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    IK_CORE_WARN("Destroying Open GL Framebuffer !!!");
  }
  
}
