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
    // Member Methods
    // ------------------
    /// This function reads the shader code in string and store all the shader present in the file in a
    /// map to be used later by compiler.
    /// - Parameter source_string: shader code in string
    void PreprocessFile(const std::string& source_string);
    /// This functions compiles all the shader codes and store their ID in Program ID (renderer_id).
    void Compile();

    // ------------------
    // Member variables
    // ------------------
    RendererID renderer_id_ = 0;
    std::string asset_path_ = "", name_ = "";
    std::unordered_map<GLenum, std::string> shader_source_code_map_;
  };
  
}
