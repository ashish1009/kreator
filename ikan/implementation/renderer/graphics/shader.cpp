//
//  shader_compiler.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "shader.hpp"
#include "platform/open_gl/open_gl_shader_compiler.hpp"

namespace ikan {
  
  std::shared_ptr<Shader> Shader::Create(const std::string &file_path) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl: return std::make_shared<OpenGLShader>(file_path);
      case Renderer::Api::None:
      default: IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Shader Library
  // --------------------------------------------------------------------------
  std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::shader_library_;
  
  std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string& path) {
    if (shader_library_.find(path) == shader_library_.end()) {
      shader_library_[path] = Shader::Create(path);
      IK_CORE_DEBUG("Adding Shader '{0}' to Shdaer Library",
                    StringUtils::GetNameFromFilePath(path));
    }
    else {
      IK_CORE_DEBUG("Returning Pre loaded Shader '{0}' from Shdaer Library", StringUtils::GetNameFromFilePath(path));
    }
    
    return shader_library_.at(path);
  }
  
  void ShaderLibrary::ResetShaders() {
    for (auto it = shader_library_.begin(); it != shader_library_.end(); it++) {
      IK_CORE_WARN("Removing Shader '{0}' from Shdaer Library",
                   StringUtils::GetNameFromFilePath(it->first));
      it->second.reset();
    }
  }

  
}
