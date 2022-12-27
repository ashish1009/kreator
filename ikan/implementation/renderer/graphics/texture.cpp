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
  // Texture Library
  // --------------------------------------------------------------------------
  std::unordered_map<std::string, std::shared_ptr<Texture>> TextureLibrary::shader_library_;
  
  std::shared_ptr<Texture> TextureLibrary::GetShader(const std::string& path,
                                                     bool min_linear,
                                                     bool mag_linear) {
    if (shader_library_.find(path) == shader_library_.end()) {
      shader_library_[path] = Texture::Create(path, min_linear, mag_linear);
      IK_CORE_DEBUG("Adding Shader '{0}' to Shdaer Library",
                    StringUtils::GetNameFromFilePath(path));
    }
    else {
      IK_CORE_DEBUG("Returning Pre loaded Shader '{0}' from Shdaer Library", StringUtils::GetNameFromFilePath(path));
    }
    
    return shader_library_.at(path);
  }
  
  void TextureLibrary::ResetShaders() {
    for (auto it = shader_library_.begin(); it != shader_library_.end(); it++) {
      IK_CORE_WARN("Removing Shader '{0}' from Shdaer Library",
                   StringUtils::GetNameFromFilePath(it->first));
      it->second.reset();
    }
  }


}
