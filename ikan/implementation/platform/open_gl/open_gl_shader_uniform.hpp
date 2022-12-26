//
//  open_gl_shader_uniform.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

#include "renderer/utils/shader_uniform.hpp"

namespace ikan {
  
  /// This class stores the shader Uniform buffer implementation. Stores the
  /// Type and count of data (single data from or outside struct)
  class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration {
  public:
    enum class Type {
      None,
      Float32,
      Vec2, Vec3, Vec4,
      Mat3, Mat4,
      Int32,
      Bool,
      Struct
    };
    
    // ------------------------------
    // Constructor and Destructors
    // ------------------------------
    /// This constructor creates the open GL Shader uniform field with
    /// fundamental type
    /// - Parameters:
    ///   - domain: Domain of shader
    ///   - type: type of uniform
    ///   - name: name of uniform
    OpenGLShaderUniformDeclaration(ShaderDomain domain,
                                   Type type,
                                   const std::string& name,
                                   uint32_t count = 1);
    /// This constructor creates the open GL Shader uniform field with
    /// structure type
    /// - Parameters:
    ///   - domain: domain of shader
    ///   - uniformStruct: structure pointer
    ///   - name: name of structure
    OpenGLShaderUniformDeclaration(ShaderDomain domain,
                                   ShaderStruct* uniformStruct,
                                   const std::string& name,
                                   uint32_t count = 1);
    
    /// This is the default destructor of Shader uniform
    virtual ~OpenGLShaderUniformDeclaration() = default;
    
    // -----------
    // Getters
    //------------
    /// This function returns the name of field
    const std::string& GetName() const override;
    /// This function returns the size of field
    uint32_t GetSize() const override;
    /// This function returns the count of field
    uint32_t GetCount() const override;
    /// This function returns the offset of field
    uint32_t GetOffset() const override;
    /// This function returns the domain of shader
    ShaderDomain GetDomain() const override;
    /// This function returns the absolute offset of field in shader.
    uint32_t GetAbsoluteOffset() const;
    /// This function returns the location of field in the shader
    int32_t GetLocation(uint32_t index = 0) const;
    /// This function returns the otype of field
    Type GetType() const;
    /// This function returns true if field is array else false
    bool IsArray() const;
    /// This function returns the structure pointer if field is structure
    const ShaderStruct& GetShaderUniformStruct() const;
    
    // -----------------
    // Static Functions
    // -----------------
    /// This static funciton returns the size of uniform field
    /// - Parameter type: type of field
    static uint32_t SizeOfUniformType(Type type);
    /// This function return the type with string type
    /// - Parameter type: typ in strug
    static Type StringToType(const std::string& type);
    /// This funciton reutrns the type in string from Type
    /// - Parameter type: Internal Type of field
    static std::string TypeToString(Type type);
    
  protected:
    /// This funciton updates the offset of field
    /// - Parameter offset: new offset
    void SetOffset(uint32_t offset) override;
    
  private:
    std::string name_;
    uint32_t size_, count_, offset_;
    
    Type type_;
    ShaderDomain domain_;
    ShaderStruct* struct_;
    
    std::vector<int32_t> location_;
    
    friend class OpenGLShader;
    friend class OpenGLShaderUniformBufferDeclaration;
  };
  
  /// Stores the type of shader Uniform Resources. Non premitive data types like Sampler
  class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration {
  public:
    enum class Type {
      kNone, kTexture2D, kTextureCubeMap
    };
    
    // ------------------------------
    // Constructor and Destructors
    // ------------------------------
    /// This constructor creates the resource uniform of shader
    /// - Parameters:
    ///   - type: type of
    ///   - name: name of uniform
    ///   - count: count of uniform
    OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count);
    /// This is default destrcutro
    virtual ~OpenGLShaderResourceDeclaration() = default;
    
    // -----------
    // Getters
    //------------
    /// This function returns the name of uniform
    const std::string& GetName() const override;
    /// This function returns the register of uniform
    uint32_t GetRegister() const override;
    /// This function returns the count of uniform
    uint32_t GetCount() const override;
    /// This function returns the type of uniform
    Type GetType() const;
    
    // ------------------
    // Static Functions
    // ------------------
    /// This static function returns the type from string
    /// - Parameter type: type in string
    static Type StringToType(const std::string& type);
    /// This static funciton returns the type of uniform resource in string
    /// - Parameter type: type of uniform resource
    static std::string TypeToString(Type type);
    
  private:
    std::string name_;
    uint32_t register_, count_;
    Type type_;
    
    friend class OpenGLShader;
  };
  
  /// Store all the Uniform in a vector
  class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration {
  public:
    // ------------------------------
    // Constructor and Destructors
    // ------------------------------
    /// This constructor create the Unfirm buffer declaration with name and domain
    /// - Parameters:
    ///   - name: name of uniform
    ///   - domain: domain of uniform
    OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);
    /// Default destructor that delete the buffer
    virtual ~OpenGLShaderUniformBufferDeclaration();
    
    // --------------
    // Fundamentals
    // -------------
    /// This function finds the uniform declaretion by name
    /// - Parameter name: name of uniform
    ShaderUniformDeclaration* FindUniform(const std::string& name) override;
    /// This function pushes the unifom in vector
    /// - Parameter uniform: uniform pointer to be pushed
    void PushUniform(OpenGLShaderUniformDeclaration* uniform);
    
    // -----------
    // Getters
    //------------
    /// This function returns the name of buffer
    const std::string& GetName() const override;
    /// This function returns the register of buffer
    uint32_t GetRegister() const override;
    /// This function returns the size of buffer
    uint32_t GetSize() const override;
    /// This function returns all the declaration in buffer
    const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const override;
    /// This function return the domain of shader
    ShaderDomain GetDomain() const;
    
  private:
    std::string name_;
    
    uint32_t register_, size_;
    
    std::vector<ShaderUniformDeclaration*> uniforms_;
    ShaderDomain domain_;
    
    friend class Shader;
    friend class OpenGLShader;
  };
  
}
