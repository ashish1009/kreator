//
//  open_gl_shader_compiler.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

#include "renderer/graphics/shader_compiler.hpp"
#include "platform/open_gl/open_gl_shader_uniform.hpp"
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
    /// This function reads and parse the shader code and extracts the structure
    /// and uniforms and store them in data
    void Parse();
    /// This function resolves all the uniform present in the shader
    void ResolveUniforms();
    
    /// This function parses the Uniforms that are structure in shader. It will
    /// just store the structures only
    /// - Parameters:
    ///   - block: block code of shader
    ///   - domain: type of shader
    void ParseUniformStruct(const std::string& block, ShaderDomain domain);
    /// This function parses the Uniform that are fundamental types (Not struct)
    /// - Parameters:
    ///   - statement: block fo code of shader
    ///   - domain domain of shader
    void ParseUniform(const std::string& statement, ShaderDomain domain);

    /// This function finds the structure stored in shader
    /// - Parameter name: Name of structure
    ShaderStruct* FindStruct(const std::string& name);

    // ------------------
    // Member variables
    // ------------------
    RendererID renderer_id_ = 0;
    std::string asset_path_ = "", name_ = "";
    std::unordered_map<GLenum, std::string> shader_source_code_map_;
    
    std::vector<ShaderStruct*> structs_; // Stores the structure in the shader
    std::vector<ShaderResourceDeclaration*> resources_; // Stores the resources of shader like sampler
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> vs_material_uniform_buffer_; // Uniform data buffer of vertex shader
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> fs_material_uniform_buffer_; // Uniform data buffer of pixel shader
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> gs_material_uniform_buffer_; // Uniform data buffer of geometry shader
  };
  
}
