//
//  open_gl_shader_compiler.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "open_gl_shader_compiler.hpp"

namespace ikan {
    
  OpenGLShader::OpenGLShader(const std::string& file_path)
  : asset_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)) {
    IK_CORE_DEBUG("Creating Open GL Shader ...");
    IK_CORE_DEBUG("  Renderer ID | {0} ", renderer_id_);
    IK_CORE_DEBUG("  Name        | {0} ", name_);
    IK_CORE_DEBUG("  File Path   | {0} ", asset_path_);
  }
  
  OpenGLShader::~OpenGLShader() noexcept {
    IK_CORE_WARN("Destroying Open GL Shader ...");
    IK_CORE_WARN("  Renderer ID | {0} ", renderer_id_);
    IK_CORE_WARN("  Name        | {0} ", name_);
    IK_CORE_WARN("  File Path   | {0} ", asset_path_);
  }

  const std::string& OpenGLShader::GetName() const { return name_; }
  const std::string& OpenGLShader::GetFilePath() const { return asset_path_; }
  RendererID OpenGLShader::GetRendererID() const { return renderer_id_; }

}
