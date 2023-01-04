//
//  texture.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "texture.hpp"
#include "platform/open_gl/open_gl_texture.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Texture Create API
  // --------------------------------------------------------------------------
  std::shared_ptr<Texture> Texture::Create(const std::string& file_path,
                                           bool min_linear,
                                           bool mag_linear) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLTexture>(file_path, min_linear, mag_linear);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  std::shared_ptr<Texture> Texture::Create(uint32_t width,
                                           uint32_t height,
                                           void* data,
                                           uint32_t size) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLTexture>(width, height, data, size);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Character texture
  // --------------------------------------------------------------------------
  std::shared_ptr<CharTexture> CharTexture::Create(const FT_Face& face,
                                                   const glm::ivec2& size,
                                                   const glm::ivec2& bearing,
                                                   uint32_t advance,
                                                   [[maybe_unused]] char char_val) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLCharTexture>(face, size, bearing, advance, char_val);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Image API
  // --------------------------------------------------------------------------
  std::shared_ptr<Image> Image::Create(uint32_t width,
                                       uint32_t height,
                                       TextureFormat format,
                                       void* data) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLImage>(width, height, format, data);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }

  // --------------------------------------------------------------------------
  // Texture Library
  // --------------------------------------------------------------------------
  std::unordered_map<std::string, std::shared_ptr<Texture>> TextureLibrary::shader_library_;
  
  std::shared_ptr<Texture> TextureLibrary::GetShader(const std::string& path,
                                                     bool min_linear,
                                                     bool mag_linear) {
    if (shader_library_.find(path) == shader_library_.end()) {
      shader_library_[path] = Texture::Create(path, min_linear, mag_linear);
      IK_CORE_DEBUG(LogModule::Texture, "Adding Shader '{0}' to Shdaer Library",
                    StringUtils::GetNameFromFilePath(path));
    } else {
      IK_CORE_DEBUG(LogModule::Texture, "Returning Pre loaded Shader '{0}' from Shdaer Library", StringUtils::GetNameFromFilePath(path));
    }
    
    return shader_library_.at(path);
  }
  
  void TextureLibrary::ResetTextures() {
    for (auto it = shader_library_.begin(); it != shader_library_.end(); it++) {
      IK_CORE_WARN(LogModule::Texture, "Removing Shader '{0}' from Shdaer Library",
                   StringUtils::GetNameFromFilePath(it->first));
      it->second.reset();
    }
  }

}
