//
//  open_gl_texture.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "open_gl_texture.hpp"
#include "renderer/utils/renderer_stats.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"

#include <stb_image.h>
#include <glad/glad.h>

namespace ikan {
  
  namespace texture_utils {
    
#ifdef IK_DEBUG_FEATURE
    
    /// This function returns the Format name from Enum
    /// - Parameter format: enum taken as uint (enum in Glad)
    static std::string GetFormatNameFromEnum(uint32_t format) {
      if (format == GL_RGBA8) return "GL_RGBA8";
      if (format == GL_RGBA)  return "GL_RGBA";
      if (format == GL_RGB8)  return "GL_RGB8";
      if (format == GL_RGB)   return "GL_RGB";
      if (format == GL_RED)   return "GL_RED";
      else IK_CORE_ASSERT(false, "Add New Featured format herer too");
    }
    
#endif
    
  } // namespace texture_utils
  
 
  OpenGLTexture::OpenGLTexture(const std::string& file_path,
                               bool min_linear,
                               bool mag_linear)
  : file_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)),
  internal_format_(GL_RGBA8), data_format_(GL_RGBA) {
    if (renderer_id_)
      glDeleteTextures(1, &renderer_id_);
    
    // Invert the texture. as by default open gl load inverted vertically
    stbi_set_flip_vertically_on_load(1);
    
    // Load the file with stb image API
    texture_data_ = stbi_load(file_path_.c_str(),
                              &width_,
                              &height_,
                              &channel_,
                              0 /* desired_channels */ );
    
    // If file loaded successfullt
    if (texture_data_) {
      uploaded_ = true;
      switch (channel_) {
        case 4 :
          internal_format_ = GL_RGBA8;
          data_format_     = GL_RGBA;
          break;
        case 3 :
          internal_format_ = GL_RGB8;
          data_format_     = GL_RGB;
          break;
        case 2 :
        case 1 :
          internal_format_ = GL_RED;
          data_format_     = GL_RED;
          break;
          
        default:
          IK_CORE_ASSERT(false, "Invalid Format ");
      }

      IDManager::GetTextureId(renderer_id_);
      
      // Setup min and Mag filter
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (min_linear ? GL_LINEAR : GL_NEAREST));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (min_linear ? GL_LINEAR : GL_NEAREST));
      
      // Texuter Flags
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      
      // Create texture in the renderer Buffer
      glTexImage2D(GL_TEXTURE_2D,
                   0, // Level
                   (GLint)internal_format_,
                   width_,
                   height_,
                   0, // Border
                   data_format_,
                   GL_UNSIGNED_BYTE,
                   texture_data_);
      size_ = (uint32_t)width_ * (uint32_t)height_ * (uint32_t)channel_;
      
      // Increment the size in stats
      RendererStatistics::Get().texture_buffer_size += size_;
      
      // Delete the data as we have already loaded in graphics
      delete (stbi_uc*)texture_data_;
    }
    
    if (uploaded_) {
      IK_CORE_DEBUG("Creating Open GL Texture from File ... ");
      IK_CORE_DEBUG("  File Path         | {0}", file_path_);
      IK_CORE_DEBUG("  Renderer ID       | {0}", renderer_id_);
      IK_CORE_DEBUG("  Width             | {0}", width_);
      IK_CORE_DEBUG("  Height            | {0}", height_);
      IK_CORE_DEBUG("  Size              | {0} Bytes ({1} KB {2})",
                    size_, (float)size_ / 1000.0f, (float)size_ / 1000000.0f);
      IK_CORE_DEBUG("  Number of Channel | {0}", channel_);
      IK_CORE_DEBUG("  InternalFormat    | {0}",
                    texture_utils::GetFormatNameFromEnum(internal_format_));
      IK_CORE_DEBUG("  DataFormat        | {0}",
                    texture_utils::GetFormatNameFromEnum(data_format_));
    }
    else {
      IK_CORE_CRITICAL("Failed to load stbi Image {0}", file_path_.c_str());
    }
  }
  
  OpenGLTexture::~OpenGLTexture() noexcept {
    if (uploaded_) {
      IK_CORE_WARN("Destroying Open GL Texture: !!! ");
      if (file_path_ != "")
        IK_CORE_WARN("  File Path         | {0}", file_path_);
      
      IK_CORE_WARN("  Renderer ID       | {0}", renderer_id_);
      IK_CORE_WARN("  Width             | {0}", width_);
      IK_CORE_WARN("  Height            | {0}", height_);
      IK_CORE_WARN("  Size              | {0} Bytes ({1} KB {2} MB)",
                   size_, (float)size_ / 1000.0f, (float)size_ / 1000000.0f);
      IK_CORE_WARN("  Number of Channel | {0}", channel_);
      IK_CORE_WARN("  InternalFormat    | {0}",
                   texture_utils::GetFormatNameFromEnum(internal_format_));
      IK_CORE_WARN("  DataFormat        | {0}",
                   texture_utils::GetFormatNameFromEnum(data_format_));
      
      IDManager::RemoveTextureId(renderer_id_);
      RendererStatistics::Get().texture_buffer_size -= size_;
    }
  }

}
