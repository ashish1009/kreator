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
  
}
