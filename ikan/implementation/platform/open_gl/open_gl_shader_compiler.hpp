//
//  open_gl_shader_compiler.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

#include "renderer/graphics/shader_compiler.hpp"
#include <glad/glad.h>

namespace ikan {
  
  /// This is the Implementation of Open GL Shader compiler
  class OpenGLShader : public Shader {
  public:
    // ---------------------------------
    // Constructors and Destructor
    // ---------------------------------
    /// This constructor creates the Open GL Shader compiler with file path
    /// - Parameter file_path: shader file path
    OpenGLShader(const std::string& file_path);
    /// This destructor destroy the Open GL Shader compiler
    ~OpenGLShader() noexcept;
    
    // --------------
    // Getters
    // --------------
    /// This function returns the Name of Shader
    const std::string& GetName() const override;
    /// This function returns the File Path of Shader
    const std::string& GetFilePath() const override;
    /// This function returns the Renderer ID of Shader
    RendererID GetRendererID() const override;


    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLShader);

  private:
    // ------------------
    // Member variables
    // ------------------
    RendererID renderer_id_ = 0;
    std::string asset_path_ = "", name_ = "";
  };
  
}
