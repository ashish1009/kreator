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
    
#ifdef IK_DEBUG_FEATURE
    /// This function returns the string name of attachment texture format
    /// - Parameter format: format type
    static std::string GetTextureFormateStringFromEnum(FrameBuffer::Attachments::TextureFormat format) {
      switch (format) {
        case FrameBuffer::Attachments::TextureFormat::RGBA8:
          return "RGBA8";
        case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
          return "Depth24Stencil";
        case FrameBuffer::Attachments::TextureFormat::None:
        default:
          IK_CORE_WARN("Framebuffer created without attachment");
          return "No Attachment";
      }
    }
#endif

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
    Invalidate();
  }

  OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    IK_CORE_WARN("Destroying Open GL Framebuffer !!!");
    IK_CORE_DEBUG("  Renderer ID | {0}", renderer_id_);
    uint32_t i = 0;
    IK_CORE_DEBUG("  Color Attachments ");
    for (auto& color_id : color_attachment_ids_) {
      IK_CORE_DEBUG("  Renderer ID | {0}", color_id);
      IK_CORE_DEBUG("  Format      | {0}", frame_buffer_utils::GetTextureFormateStringFromEnum(color_specifications_[i++]));
      IDManager::RemoveTextureId(color_id);
    }
    
    IK_CORE_DEBUG("  Depth Attachments ");
    IK_CORE_DEBUG("  Renderer ID | {0}", depth_attachment_id_);
    IK_CORE_DEBUG("  Format      | {0}", frame_buffer_utils::GetTextureFormateStringFromEnum(depth_specification_));
    IDManager::RemoveTextureId(depth_attachment_id_);
    
    IDManager::RemoveFramebufferId(renderer_id_);
  }
  
  void OpenGLFrameBuffer::Invalidate() {
    // Delete the framebuffers if already created
    if (renderer_id_) {
      IDManager::RemoveFramebufferId(renderer_id_);

      // Remove color attachments
      for (uint32_t i = 0; i < color_specifications_.size(); i++)
        IDManager::RemoveTextureId(color_attachment_ids_[i]);

      // Remove depth attachment
      IDManager::RemoveTextureId(depth_attachment_id_);

      color_attachment_ids_.clear();
      depth_attachment_id_ = -1;
    }
    
    // Generate the frame buffer to renderer ID
    IDManager::GetFramebufferId(renderer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

    IK_CORE_DEBUG("Invalidating Open GL Framebuffer | {0}", renderer_id_);
  }
}
