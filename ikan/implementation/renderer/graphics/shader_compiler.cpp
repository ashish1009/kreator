//
//  shader_compiler.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "shader_compiler.hpp"
#include "platform/open_gl/open_gl_shader_compiler.hpp"

namespace ikan {
  
  std::shared_ptr<Shader> Shader::Create(const std::string &file_path) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl: return std::make_shared<OpenGLShader>(file_path);
      case Renderer::Api::None:
      default: IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
}
