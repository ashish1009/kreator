//
//  shader_compiler.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  // Forward Declaration
  class ShaderUniformBufferDeclaration;
  class ShaderResourceDeclaration;

  /// This is interface class to complile graphics shader
  class Shader {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// This is the default virtual destructor
    virtual ~Shader() noexcept = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the Current Shader to GPU
    virtual void Bind() const = 0;
    /// This function unbinds the Current Shader to GPU
    virtual void Unbind() const = 0;
    
    // Shader Uniforms
    /// This function set the vertex shader buffer data
    /// - Parameter buffer: buffer data
    virtual void SetVSMaterialUniformBuffer(const Buffer& buffer) = 0;
    /// This function set the fragment shader buffer data
    /// - Parameter buffer: buffer data
    virtual void SetFSMaterialUniformBuffer(const Buffer& buffer) = 0;
    /// This function set the geomatry shader buffer data
    /// - Parameter buffer: buffer data
    virtual void SetGSMaterialUniformBuffer(const Buffer& buffer) = 0;
    
    // -------------
    // Getters
    // -------------
    /// This function returns the Name of Shader
    virtual const std::string& GetName() const = 0;
    /// This function returns the File Path of Shader
    virtual const std::string& GetFilePath() const = 0;
    /// This function returns the Renderer ID of Shader
    virtual RendererID GetRendererID() const = 0;
    
    /// This function returns true if have the vertex shader buffer data
    virtual bool HasVSMaterialUniformBuffer() const = 0;
    /// This function returns true if have the fragment shader buffer data
    virtual bool HasFSMaterialUniformBuffer() const = 0;
    /// This function returns true if have the geomatry shader buffer data
    virtual bool HasGSMaterialUniformBuffer() const = 0;
    
    /// This function resturns the vertex Shader buffer data
    virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
    /// This function resturns the fragment Shader buffer data
    virtual const ShaderUniformBufferDeclaration& GetFSMaterialUniformBuffer() const = 0;
    /// This function resturns the geomatry Shader buffer data
    virtual const ShaderUniformBufferDeclaration& GetGSMaterialUniformBuffer() const = 0;
    
    /// This function returns all the resources
    virtual const std::vector<ShaderResourceDeclaration*>& GetResources() const = 0;
    
    // -----------
    // Attributs
    // -----------
    
    // NOTE: For Int and Int arrar (for Sampler 2D) Unform is setting while
    // compiling the shader so no need to call this explicitly
    
    /// This functions uploads the Matrix 4x4 array value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - values: Value of Uniform
    ///   - count: Size of Mat4 Array
    virtual void SetUniformMat4Array(const std::string& name,
                                     const glm::mat4& values,
                                     uint32_t count) = 0;
    
    /// This functions uploads the Matrix 4x4 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformMat4(const std::string& name,
                                const glm::mat4& value) = 0;
    /// This functions uploads the Matrix 3x3 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformMat3(const std::string& name,
                                const glm::mat3& value) = 0;
    
    /// This functions uploads the Flaot value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformFloat1(const std::string& name,
                                  float value) = 0;
    /// This functions uploads the Vec2 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformFloat2(const std::string& name,
                                  const glm::vec2& value) = 0;
    /// This functions uploads the Vec3 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformFloat3(const std::string& name,
                                  const glm::vec3& value) = 0;
    /// This functions uploads the Vec4 value to shader
    /// - Parameters:
    ///   - name: Name of Uniform
    ///   - value: Value of Uniform
    virtual void SetUniformFloat4(const std::string& name,
                                  const glm::vec4& value) = 0;
    
    // ---------------
    // Static Create
    // ---------------
    /// This static function creates Shader Instance based on the Suported API
    /// - Parameter path: Absolute Path of shader
    static std::shared_ptr<Shader> Create(const std::string& file_path);
  };
  
  /// This class stores the compiled shader in library
  class ShaderLibrary {
  private:
    // -----------
    // Functions
    // -----------
    /// This function returns the Ref type of ikan::Shader. It creates a new if not present in the map
    /// - Parameter path: path of shader
    static std::shared_ptr<Shader> GetShader(const std::string& path);
    /// This function deletes all the shaders present int the map
    static void ResetShaders();
    
    // -----------
    // Variables
    // -----------
    static std::unordered_map<std::string, std::shared_ptr<Shader>> shader_library_;
    
    MAKE_PURE_STATIC(ShaderLibrary)
    
    friend class Renderer;
  };

  
}
