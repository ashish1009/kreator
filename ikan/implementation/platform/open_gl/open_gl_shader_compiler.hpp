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
    // -------------
    // Constants
    // -------------
    static constexpr uint32_t MaxShaderSupported = 3;

    // ---------------------------------
    // Constructors and Destructor
    // ---------------------------------
    /// This constructor creates the Open GL Shader compiler with file path
    /// - Parameter file_path: shader file path
    OpenGLShader(const std::string& file_path);
    /// This destructor destroy the Open GL Shader compiler
    ~OpenGLShader() noexcept;
    
    // -----------------
    // Fundamentals
    // -----------------
    /// This function binds the Current Shader to GPU
    void Bind() const override;
    /// This function unbinds the Current Shader to GPU
    void Unbind() const override;
    
    // -----------------
    // Shader Uniforms
    // -----------------
    /// This function set the vertex shader buffer data
    /// - Parameter buffer: buffer data
    void SetVSMaterialUniformBuffer(const Buffer& buffer) override;
    /// This function set the fragment shader buffer data
    /// - Parameter buffer: buffer data
    void SetFSMaterialUniformBuffer(const Buffer& buffer) override;
    /// This function set the geomatry shader buffer data
    /// - Parameter buffer: buffer data
    void SetGSMaterialUniformBuffer(const Buffer& buffer) override;

    
    // --------------
    // Getters
    // --------------
    /// This function returns the Name of Shader
    const std::string& GetName() const override;
    /// This function returns the File Path of Shader
    const std::string& GetFilePath() const override;
    /// This function returns the Renderer ID of Shader
    RendererID GetRendererID() const override;

    /// This function returns true if have the vertex shader buffer data
    bool HasVSMaterialUniformBuffer() const override;
    /// This function returns true if have the fragment shader buffer data
    bool HasFSMaterialUniformBuffer() const override;
    /// This function returns true if have the geomatry shader buffer data
    bool HasGSMaterialUniformBuffer() const override;
    
    /// This function resturns the vertex Shader buffer data
    const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override;
    /// This function resturns the fragment Shader buffer data
    const ShaderUniformBufferDeclaration& GetFSMaterialUniformBuffer() const override;
    /// This function resturns the geomatry Shader buffer data
    const ShaderUniformBufferDeclaration& GetGSMaterialUniformBuffer() const override;
    
    /// This function returns all the resources
    const std::vector<ShaderResourceDeclaration*>& GetResources() const override;
    
    // -----------
    // Attributs
    // -----------
    /// This functions uploads the Matrix 4x4 array value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - values: Value of Uniform
    ///   - count: Size of Mat4 Array
    void SetUniformMat4Array(const std::string& name,
                             const glm::mat4& values,
                             uint32_t count) override;
    
    /// This functions uploads the Matrix 4x4 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformMat4(const std::string& name,
                        const glm::mat4& value) override;
    /// This functions uploads the Matrix 3x3 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformMat3(const std::string& name,
                        const glm::mat3& value) override;
    
    /// This functions uploads the Flaot value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat1(const std::string& name,
                          float value) override;
    /// This functions uploads the Vec2 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat2(const std::string& name,
                          const glm::vec2& value) override;
    /// This functions uploads the Vec3 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat3(const std::string& name,
                          const glm::vec3& value) override;
    /// This functions uploads the Vec4 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformFloat4(const std::string& name,
                          const glm::vec4& value) override;

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

    /// This function returns the location of attribute in the shader
    /// - Parameter name: attribute name
    int32_t GetUniformLocation(const std::string& name);

    /// This function resolves and set the uniforms using declaration
    /// - Parameters:
    ///   - decl: declaration
    ///   - buffer: buffer with data
    void ResolveAndSetUniforms(const std::shared_ptr<OpenGLShaderUniformBufferDeclaration>& decl,
                               const Buffer& buffer);
    /// This function resolves and set the uniforms using uniform instance
    /// - Parameters:
    ///   - uniform: uniform
    ///   - buffer: buffer with data
    void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform,
                              const Buffer& buffer);
    /// This function resolves and set the uniforms using uniform instance array
    /// - Parameters:
    ///   - uniform: uniform
    ///   - buffer: buffer with data
    void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform,
                                   const Buffer& buffer);
    /// This function resolves and set the uniforms using field instance
    /// - Parameters:
    ///   - field: field isntacnce
    ///   -  data: data buffer
    ///   - offset: offset of data
    ///   - idx: index
    void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field,
                                   std::byte* data,
                                   int32_t offset,
                                   uint8_t idx);
    
    // Attributes
    /// This functions uploads the Int value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    void SetUniformInt1(const std::string& name,
                        int32_t value);
    /// This functions uploads the Int Array value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - values: Values of Uniform
    ///   - count: Size of array
    void SetIntArray(const std::string& name,
                     int32_t* values,
                     uint32_t count);
    
    /// This function upload the uniform of type int
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformInt1(int32_t location,
                           int32_t value);
    
    /// This function upload the uniform of type mat 4
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformMat4(int32_t location,
                           const glm::mat4& value);
    /// This function upload the uniform of type mat3
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformMat3(int32_t location,
                           const glm::mat3& value);
    
    /// This function upload the uniform of type flaot
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformFloat1(int32_t location,
                             float value);
    /// This function upload the uniform of type vec2
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformFloat2(int32_t location,
                             const glm::vec2& value);
    /// This function upload the uniform of type vec3
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformFloat3(int32_t location,
                             const glm::vec3& value);
    /// This function upload the uniform of type vec4
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformFloat4(int32_t location,
                             const glm::vec4& value);
    
    /// This function upload the uniform of type mat4 array
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformMat4Array(uint32_t location,
                                const glm::mat4& values,
                                uint32_t count);
    /// This function upload the uniform of type structure
    /// - Parameters:
    ///   - location: location of field
    ///   - value: value to be uploaded
    void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform,
                             std::byte* buffer,
                             uint32_t offset);
    // ------------------
    // Member variables
    // ------------------
    RendererID renderer_id_ = 0;
    std::string asset_path_ = "", name_ = "";
    std::unordered_map<GLenum, std::string> shader_source_code_map_;
    std::unordered_map<std::string, int32_t> location_map_;

    std::vector<ShaderStruct*> structs_; // Stores the structure in the shader
    std::vector<ShaderResourceDeclaration*> resources_; // Stores the resources of shader like sampler
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> vs_material_uniform_buffer_; // Uniform data buffer of vertex shader
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> fs_material_uniform_buffer_; // Uniform data buffer of pixel shader
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> gs_material_uniform_buffer_; // Uniform data buffer of geometry shader
  };
  
}
