//
//  shader_uniform.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

namespace ikan {
  
  /// This enum stores the type of shader
  enum class ShaderDomain {
    None = 0,
    Vertex = 1,
    Fragment = 2,
    Geometry = 3
  };
  
  /// This class stores shader uniform elements (like int, float, vec ....)
  class ShaderUniformDeclaration {
  private:
    friend class Shader;
    friend class OpenGLShader;
    friend class ShaderStruct;
    
  public:
    virtual ~ShaderUniformDeclaration() = default;
    
    /// This function returns the name of field
    virtual const std::string& GetName() const = 0;
    /// This functionr returns the size of field
    virtual uint32_t GetSize() const = 0;
    /// This function returns the count of field
    virtual uint32_t GetCount() const = 0;
    /// This function returns the offset of field
    virtual uint32_t GetOffset() const = 0;
    
    /// This function returns the domain of shader
    virtual ShaderDomain GetDomain() const = 0;
    
  protected:
    /// This function update the offset of the field
    virtual void SetOffset(uint32_t offset) = 0;
  };
  
  class ShaderUniformBufferDeclaration {
  public:
    /// This is the defualt destructor
    virtual ~ShaderUniformBufferDeclaration() = default;
    
    /// This function returns the name of buffer
    virtual const std::string& GetName() const = 0;
    /// This function returns the register of buffer
    virtual uint32_t GetRegister() const = 0;
    /// This function returns the size of buffer
    virtual uint32_t GetSize() const = 0;
    /// This function returns all the declaration in buffer
    virtual const std::vector<ShaderUniformDeclaration*>& GetUniformDeclarations() const = 0;
    /// This function finds the uniform declaretion by name
    /// - Parameter name: name of uniform
    virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
  };
  
  /// This class stores the uniform resource like sampler 2D ...
  class ShaderResourceDeclaration {
  public:
    /// This is the default destructor
    virtual ~ShaderResourceDeclaration() = default;
    
    /// This function returns the name of resrouce
    virtual const std::string& GetName() const = 0;
    /// This function returns the register ot resource
    virtual uint32_t GetRegister() const = 0;
    /// This function returns the count of array of resource
    virtual uint32_t GetCount() const = 0;
  };
  
  /// This class stores the content of a structure
  class ShaderStruct {
  public:
    /// This constructor creates the structure instance for a shader
    /// - Parameter name: name of structure
    ShaderStruct(const std::string& name);
    /// Destroy the structure
    ~ShaderStruct();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ShaderStruct)
    
    /// This function updates the offset value of structure
    /// - Parameter offset: new offset
    void SetOffset(uint32_t offset);
    
    /// This function returns the size of structure in bytes
    uint32_t GetSize() const;
    /// This function returns the offset of structure
    uint32_t GetOffset() const;
    
    /// This function returns the name of structure
    const std::string& GetName() const;
    /// This function returns the all the fields of structure
    const std::vector<ShaderUniformDeclaration*>& GetFields() const;
    
    /// This function adds a new field in structure
    void AddField(ShaderUniformDeclaration* field);
    
  private:
    std::string name_;
    
    uint32_t size_ = 0;
    uint32_t offset_= 0;
    
    // Stores each field of structure
    std::vector<ShaderUniformDeclaration*> fields_;
    
    friend class OpenGLShader;
    friend class Shader;
  };

}
