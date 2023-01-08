//
//  open_gl_framebuffer.cpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#include "open_gl_framebuffer.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"
#include "platform/open_gl/open_gl_texture.hpp"
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
        case FrameBuffer::Attachments::TextureFormat::R32I:
          return "R32I";
        case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
          return "Depth24Stencil";
        case FrameBuffer::Attachments::TextureFormat::None:
        default:
          IK_CORE_WARN(LogModule::FrameBuffer, "Framebuffer created without attachment");
          return "No Attachment";
      }
    }
#endif

    /// This function checks is the specification format is depth or not
    /// - Parameter format: Format type
    static bool IsDepthFormat(FrameBuffer::Attachments::TextureFormat format) {
      switch (format) {
        case FrameBuffer::Attachments::TextureFormat::RGBA8:
        case FrameBuffer::Attachments::TextureFormat::R32I:
          return false;
        case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
          return true;
          
        case FrameBuffer::Attachments::TextureFormat::None:
        default:
          IK_CORE_ASSERT(false, "invalid format");
      }
    }

    /// This function gnerates the texture ID for framebuffer
    /// - Parameters:
    ///   - out_ids: pointer of renderer IDs (number of id to be created)
    ///   - count: Number of Tecxture to be created
    static void CreateTextures(uint32_t* out_ids, uint32_t count = 1) {
      IDManager::GetTextureId(out_ids, count);
    }

    /// This funtion bind the texture attachment created in Framebuffer
    /// - Parameter id: Renderer ID to be attached
    static void BindTexture(uint32_t id) {
      glBindTexture(GL_TEXTURE_2D, id);
    }

    /// This function attaches the color/depth attachment to Frame buffer
    /// - Parameters:
    ///   - internal_format: Texture Format
    ///   - attachment_type: attachment type
    ///   - width: Width of FB
    ///   - height: Height of FB
    static void AttachTexture(GLenum internal_format,
                              GLenum attachment_type,
                              uint32_t width,
                              uint32_t height) {
      GLenum type = texture_utils::GetTextureType(internal_format);
      
      glTexImage2D(
                   GL_TEXTURE_2D,               // target
                   0,                           // level
                   (GLint)internal_format,      // internal_format
                   (GLsizei)width,              // width
                   (GLsizei)height,             // height
                   0,                           // border
                   attachment_type,             // format
                   (GLenum)type,                // type
                   nullptr                      // pixels
                   );
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    /// This function attaches Frame buufer texture 2D in framebuffer
    /// - Parameters:
    ///   - id: Renderer ID
    ///   - attachment_id: Attachment id
    ///   - type: type of attachment
    static void FramebufferTexture(RendererID id,
                                   int32_t attachment_id,
                                   int32_t type) {
      
      glFramebufferTexture2D(
                             GL_FRAMEBUFFER,        // target
                             (GLenum)attachment_id, // attachment ID
                             (GLenum)type,          // textarget TODO: Handle multi sample later
                             id,                    // texture ID
                             0                      // level
                             );
    }
    
  }
  
  // --------------------------------------------------------------------------
  // Open GL FrameBuffer API
  // --------------------------------------------------------------------------
  OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBuffer::Specification& specs)
  : specification_(specs) {
    IK_CORE_DEBUG(LogModule::FrameBuffer, "Creating Open GL Framebuffer ...");
    
    for (auto attchment_format : specification_.attachments.texture_formats) {
      if (frame_buffer_utils::IsDepthFormat(attchment_format))
        depth_specification_ = attchment_format;
      else
        color_specifications_.emplace_back(attchment_format);
    }
    Invalidate();
  }

  OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    IK_CORE_WARN(LogModule::FrameBuffer, "Destroying Open GL Framebuffer !!!");
    IK_CORE_WARN(LogModule::FrameBuffer, "  Renderer ID | {0}", renderer_id_);
    uint32_t i = 0;
    IK_CORE_WARN(LogModule::FrameBuffer, "  Color Attachments ");
    for (auto& color_id : color_attachment_ids_) {
      IK_CORE_WARN(LogModule::FrameBuffer, "  Renderer ID | {0}", color_id);
      IK_CORE_WARN(LogModule::FrameBuffer, "  Format      | {0}", frame_buffer_utils::GetTextureFormateStringFromEnum(color_specifications_[i++]));
    }
    IDManager::RemoveTextureId(&color_attachment_ids_[0], (uint32_t)color_attachment_ids_.size());

    IK_CORE_WARN(LogModule::FrameBuffer, "  Depth Attachments ");
    IK_CORE_WARN(LogModule::FrameBuffer, "  Renderer ID | {0}", depth_attachment_id_);
    IK_CORE_WARN(LogModule::FrameBuffer, "  Format      | {0}", frame_buffer_utils::GetTextureFormateStringFromEnum(depth_specification_));
    IDManager::RemoveTextureId(&depth_attachment_id_);
    
    IDManager::RemoveFramebufferId(renderer_id_);
  }
  
  void OpenGLFrameBuffer::Invalidate() {
    // Delete the framebuffers if already created
    if (renderer_id_) {
      IDManager::RemoveFramebufferId(renderer_id_);

      // Remove color attachments
      IDManager::RemoveTextureId(&color_attachment_ids_[0], (uint32_t)color_attachment_ids_.size());

      // Remove depth attachment
      IDManager::RemoveTextureId(&depth_attachment_id_);

      color_attachment_ids_.clear();
      depth_attachment_id_ = -1;
    }
    
    // Generate the frame buffer to renderer ID
    IDManager::GetFramebufferId(renderer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

    IK_CORE_DEBUG(LogModule::FrameBuffer, "Invalidating Open GL Framebuffer | {0}", renderer_id_);
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  ---------------------------------------------------------");
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  Renderer ID | {0}", renderer_id_);
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  Width       | {0}", specification_.width);
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  Height      | {0}", specification_.height);
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  Color       | {0} : {1} : {2} : {3}", specification_.color.r, specification_.color.g, specification_.color.b, specification_.color.a);
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  ---------------------");
    
    int32_t internal_format = 0;
    int32_t data_format = 0;
    
    // Color Attachments
    if (color_specifications_.size()) {
      color_attachment_ids_.resize(color_specifications_.size());
      IK_CORE_DEBUG(LogModule::FrameBuffer, "  Creating Color Texture Specifications to FrameBuffer");

      frame_buffer_utils::CreateTextures(&color_attachment_ids_[0], uint32_t(color_attachment_ids_.size()));
      for (size_t i = 0; i < color_attachment_ids_.size(); i++) {
        frame_buffer_utils::BindTexture(color_attachment_ids_[i]);
        
        IK_CORE_DEBUG(LogModule::FrameBuffer, "    Renderer ID    | {0}", color_attachment_ids_[i]);
        switch (color_specifications_[i]) {
          case FrameBuffer::Attachments::TextureFormat::None:
          case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
            break;
            
          case FrameBuffer::Attachments::TextureFormat::RGBA8:
            internal_format = GL_RGBA8;
            data_format = GL_RGBA;
            frame_buffer_utils::AttachTexture(
                                              internal_format,
                                              data_format,
                                              specification_.width,
                                              specification_.height
                                              );
            frame_buffer_utils::FramebufferTexture(
                                                   color_attachment_ids_[i],
                                                   GL_COLOR_ATTACHMENT0 + (int32_t)i,
                                                   GL_TEXTURE_2D
                                                   );

            IK_CORE_DEBUG(LogModule::FrameBuffer, "    InternalFormae | {0}", texture_utils::GetFormatNameFromEnum(internal_format));
            IK_CORE_DEBUG(LogModule::FrameBuffer, "    DataFormat     | {0}", texture_utils::GetFormatNameFromEnum(data_format));
            break;
          case FrameBuffer::Attachments::TextureFormat::R32I:
            internal_format = GL_R32I;
            data_format = GL_RED_INTEGER;
            frame_buffer_utils::AttachTexture(
                                              internal_format,
                                              data_format,
                                              specification_.width,
                                              specification_.height
                                              );
            frame_buffer_utils::FramebufferTexture(
                                                   color_attachment_ids_[i],
                                                   GL_COLOR_ATTACHMENT0 + (int32_t)i,
                                                   GL_TEXTURE_2D
                                                   );
            
            pixel_id_index_ = (uint32_t)i;
            
            IK_CORE_DEBUG(LogModule::FrameBuffer, "    InternalFormae | {0}", texture_utils::GetFormatNameFromEnum(internal_format));
            IK_CORE_DEBUG(LogModule::FrameBuffer, "    DataFormat     | {0}", texture_utils::GetFormatNameFromEnum(data_format));

        }; // switch (color_specifications_[i])
      } // for (size_t i = 0; i < color_attachment_ids_.size(); i++)
    } // if (color_specifications_.size())
    
    // Depth Attachment
    if (depth_specification_ != FrameBuffer::Attachments::TextureFormat::None) {
      IK_CORE_DEBUG(LogModule::FrameBuffer, "  Creating Depth Texture Specification to FrameBuffer");
      
      frame_buffer_utils::CreateTextures(&depth_attachment_id_);
      IK_CORE_DEBUG(LogModule::FrameBuffer, "    Renderer ID    | {0}", depth_attachment_id_);
      
      switch (depth_specification_) {
        case FrameBuffer::Attachments::TextureFormat::None:
        case FrameBuffer::Attachments::TextureFormat::RGBA8:
        case FrameBuffer::Attachments::TextureFormat::R32I:
          break;
          
        case FrameBuffer::Attachments::TextureFormat::Depth24Stencil:
          internal_format = GL_DEPTH_COMPONENT;
          data_format = GL_DEPTH_COMPONENT;
          frame_buffer_utils::BindTexture(depth_attachment_id_);
          frame_buffer_utils::AttachTexture(
                                            internal_format,
                                            data_format,
                                            specification_.width,
                                            specification_.height
                                            );
          frame_buffer_utils::FramebufferTexture(
                                                 depth_attachment_id_,
                                                 GL_DEPTH_ATTACHMENT,
                                                 GL_TEXTURE_2D
                                                 );
          IK_CORE_DEBUG(LogModule::FrameBuffer, "    InternalFormae | {0}", texture_utils::GetFormatNameFromEnum(internal_format));
          IK_CORE_DEBUG(LogModule::FrameBuffer, "    DataFormat     | {0}", texture_utils::GetFormatNameFromEnum(data_format));
          break;
      }; // switch (depth_specification_)
    } // if (depth_specification_ != FrameBuffer::Attachments::TextureFormat::None)
    IK_CORE_DEBUG(LogModule::FrameBuffer, "  ---------------------------------------------------------");

    // Error check
    if (color_attachment_ids_.size() >= 1) {
      IK_CORE_ASSERT((color_attachment_ids_.size() <= 4), "Inalid Attachment");
      static GLenum buffers[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
      };
      
      glDrawBuffers((GLsizei)color_attachment_ids_.size(), buffers);
      IK_CORE_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "FrameBuffer is Incomplete ");
    }
    else if (color_specifications_.empty() and depth_specification_ != FrameBuffer::Attachments::TextureFormat::None) {
      // Only depth-pass
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      
      static float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
      
      IK_CORE_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE),
                     "FrameBuffer is Incomplete ");
    } else {
      IK_CORE_WARN(LogModule::FrameBuffer, "Framebuffer created without attachment");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void OpenGLFrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
    Renderer::SetViewport(specification_.width, specification_.height);
  }
  
  void OpenGLFrameBuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height) {
    specification_.width  = width;
    specification_.height = height;
    
    IK_CORE_DEBUG(LogModule::FrameBuffer, "Resizing the Framebuffer");
    Invalidate();
    
    Renderer::SetViewport(specification_.width, specification_.height);
  }
  
  const FrameBuffer::Specification& OpenGLFrameBuffer::GetSpecification() const {
    return specification_;
  }
  RendererID OpenGLFrameBuffer::GetRendererId() const {
    return renderer_id_;
  }
  RendererID OpenGLFrameBuffer::GetDepthAttachmentId() const {
    return depth_attachment_id_;
  }
  const std::vector<RendererID>& OpenGLFrameBuffer::GetColorAttachmentIds() const {
    return color_attachment_ids_;
  }
  void OpenGLFrameBuffer::UpdateSpecificationColor(const glm::vec4& color) {
    specification_.color = color;
  }
  uint32_t OpenGLFrameBuffer::GetPixelIdIndex() const {
    return pixel_id_index_;
  }
  
}
