//
//  open_gl_shader_compiler.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "open_gl_shader_compiler.hpp"
#include "platform/open_gl/open_gl_renderer_id_manager.hpp"

namespace ikan {
  
  namespace shader_utils {
    
#ifdef IK_DEBUG_FEATURE
    /// This function returns the Open GL Shader Name from Open GL Type
    /// - Parameter type: type of shader in GL enum
    static const std::string ShaderNameFromType(GLenum type) {
      if (type == GL_VERTEX_SHADER) return "Vertex";
      if (type == GL_FRAGMENT_SHADER) return "Fragment";
      if (type == GL_GEOMETRY_SHADER) return "Geomatry";
      IK_CORE_ASSERT(false, "Unknown shader type!");
    }
    
    /// This function returns the Open GL Shader Name from Open GL Type
    /// - Parameter type: type of shader in GL enum
    static const std::string ShaderNameFromType(ShaderDomain type) {
      switch (type) {
        case ShaderDomain::Vertex: return "Vertex";
        case ShaderDomain::Geometry: return "Geometry";
        case ShaderDomain::Fragment: return "Fragment";
        case ShaderDomain::None:
        default: IK_CORE_ASSERT(false, "Invalid domain");
      }
    }

    /// This funciton returns the Open GL Shader Name from Type
    /// - Parameter type: type of shader in GL enum
    static const std::string ShaderNameFromInternalType(ShaderDomain type) {
      switch (type) {
        case ShaderDomain::Vertex: return "Vertex";
        case ShaderDomain::Geometry: return "Geometry";
        case ShaderDomain::Fragment: return "Fragment";
        case ShaderDomain::None:
        default: IK_CORE_ASSERT(false, "Invalid domain");
      }
    }
#endif
    
    /// This function returns the Sgader type in Open GL Format enum
    /// - Parameter type: type of shader in string (e.g. "vertex" or "fragment" ... )
    static GLenum ShaderTypeFromString(const std::string& type) {
      if ("vertex" == type)   return GL_VERTEX_SHADER;
      if ("fragment" == type) return GL_FRAGMENT_SHADER;
      if ("geometry" == type) return GL_GEOMETRY_SHADER;
      IK_CORE_ASSERT(false, "Unknown shader type!");
    }
    
    /// This function returns Open GL shader domain typy from internal type
    /// - Parameter domain: internal domain type
    static ShaderDomain GlDomainToShaderDomain(GLint domain) {
      if (domain == GL_VERTEX_SHADER)         return ShaderDomain::Vertex;
      else if (domain == GL_FRAGMENT_SHADER)  return ShaderDomain::Fragment;
      else if (domain == GL_GEOMETRY_SHADER)  return ShaderDomain::Geometry;
      else IK_CORE_ASSERT(false, "Invalid domain");
    }
    
    /// This function returns true if type is of resource
    /// - Parameter type: type of field
    static bool IsTypeStringResource(const std::string& type) {
      if (type == "sampler2D")          return true;
      if (type == "sampler2DMS")        return true;
      if (type == "samplerCube")        return true;
      if (type == "sampler2DShadow")    return true;
      return false;
    }
    
  } // namespace shader_utils
    
  OpenGLShader::OpenGLShader(const std::string& file_path)
  : asset_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)) {
    IDManager::GetShaderId(renderer_id_);

    IK_CORE_DEBUG("Creating Open GL Shader ...");
    IK_CORE_DEBUG("  Renderer ID | {0} ", renderer_id_);
    IK_CORE_DEBUG("  Name        | {0} ", name_);
    IK_CORE_DEBUG("  File Path   | {0} ", asset_path_);
    
    // Extract and Compile the Shader
    std::string file_string = StringUtils::ReadFromFile(asset_path_);
    IK_CORE_ASSERT(file_string != "", "File Not exist");
    
    PreprocessFile(file_string);
    Compile();
    
    // Parse and Store all the Uniform in Shader
    Parse();
    ResolveUniforms();
  }
  
  OpenGLShader::~OpenGLShader() noexcept {
    IK_CORE_WARN("Destroying Open GL Shader ...");
    IK_CORE_WARN("  Renderer ID | {0} ", renderer_id_);
    IK_CORE_WARN("  Name        | {0} ", name_);
    IK_CORE_WARN("  File Path   | {0} ", asset_path_);
    
    for (auto& structure : structs_)
      delete structure;
    
    for (auto& resource : resources_)
      delete resource;
  }
  
  void OpenGLShader::PreprocessFile(const std::string &source_string) {
    IK_CORE_DEBUG("  Processing the Open GL Shader: '{0}'", name_.c_str());
    
    // All type (Fragment, Vertex, Geomatry etc...) of shader code should started
    // by this token (e.g. '#type vertex' for vertex shader)
    const char* token = "#type";
    
    size_t token_pos = source_string.find(token);
    while (token_pos != std::string::npos) {
      // Find end of line of "#type <type>"
      size_t token_eol = source_string.find_first_of("\r\n", token_pos);
      IK_CORE_ASSERT((token_eol != std::string::npos), "Syntax error");
      
      // points after the token "#type " e.g. "#type vertex" it will point at 'v'
      size_t type_begin = token_pos + strlen(token) + 1; // Just after the "#type "
      
      // store the type of shader (either vertex or Fragment or Geomatry etc )
      // from 'v' to 'x' in e.g "#type vertex"
      std::string type = source_string.substr(type_begin, token_eol - type_begin);
      IK_CORE_ASSERT(shader_utils::ShaderTypeFromString(type), "Invalid shader type specified");
      
      // First line of Shader code. This is just next line after (#type <name>)
      size_t shader_code_first_line = source_string.find_first_of("\r\n", token_eol);
      
      // Finds the position of next "#type", which is the start of next shader
      // code and end of current shader code. if not found the word "#type" that
      // means this is the last shade code so find the eof
      token_pos = source_string.find(token, shader_code_first_line);
      
      // Extract the code string from 'shader_code_first_line' (which is the
      // first line of shader code) to the end of current shader code using variable
      // 'token_pos' (either next token or eol) and store the code in as string in map
      shader_source_code_map_[shader_utils::ShaderTypeFromString(type)] =
      source_string.substr(
                           shader_code_first_line,
                           token_pos - (shader_code_first_line ==  std::string::npos ?
                                  source_string.size() - 1 :
                                  shader_code_first_line)
                           );
    }
    
    // if Not able to read the file then no shader is loaded in the map
    IK_CORE_ASSERT(shader_source_code_map_.size(), "Shader source empty. No Shader exist");
  }
  
  void OpenGLShader::Compile() {
    IK_CORE_DEBUG("  Compiling Open GL Shader with File name: '{0}' ", name_.c_str());
    
    // ----------------------------------------------
    // Check Mendatory Shaders : Vertex and Fragment
    // ----------------------------------------------
    IK_CORE_ASSERT(shader_source_code_map_.find(GL_VERTEX_SHADER) != shader_source_code_map_.end(),
                   "Vertex Shader not loaded in file");
    
    IK_CORE_ASSERT(shader_source_code_map_.find(GL_FRAGMENT_SHADER) != shader_source_code_map_.end(),
                   "Fragment Shader not loaded in file");
    
    std::vector<GLuint> shader_ids;
    // -------------------
    // Shader Compiler
    // -------------------
    for (const auto& [shader_type, shader_src] : shader_source_code_map_) {
      GLuint shader = glCreateShader(shader_type);

      // Attch the shader source and then compile
      const char* shader_string = shader_src.c_str();
      glShaderSource(shader, 1, &shader_string, nullptr);
      glCompileShader(shader);

      // Shader Error Handling
      GLint is_compiled = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
      if (is_compiled == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<GLchar> info_log((size_t)max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

        glDeleteShader(shader);

        IK_CORE_ERROR("{0}", info_log.data());
        IK_CORE_ASSERT(false, "Shader compilation failure!");
      } // Error Check for shader Compiler

      // Attach both shader and link them if compiled successfully
      glAttachShader(renderer_id_, shader);
      shader_ids.push_back(shader);

      IK_CORE_DEBUG("    Compiled '{0}' Shader ", shader_utils::ShaderNameFromType(shader_type).c_str());
    }

    // -------------------
    // Shader Linking
    // -------------------
    glLinkProgram(renderer_id_);

    // Shader Error Handling
    // Note the different functions here: glGetProgram* instead of glGetShader
    GLint is_linked = 0;
    glGetProgramiv(renderer_id_, GL_LINK_STATUS, (int32_t*)&is_linked);

    if (is_linked == GL_FALSE) {
      GLint max_length = 0;
      glGetProgramiv(renderer_id_, GL_INFO_LOG_LENGTH, &max_length);

      // The maxLength includes the NULL character
      std::vector<GLchar> info_log((size_t)max_length);
      glGetProgramInfoLog(renderer_id_, max_length, &max_length, &info_log[0]);

      // We don't need the program anymore.
      glDeleteProgram(renderer_id_);

      for (auto id : shader_ids)
        glDeleteShader(id);

      IK_CORE_ERROR("{0}", info_log.data());
      IK_CORE_ASSERT(false, "Shader link failure!");
    } // Error check of Shader Linker

    // Delete all shader as we have already linked them to our shader program
    for (auto id : shader_ids)
      glDeleteShader(id);
  }
  
  void OpenGLShader::Parse() {
    // Clear all the data before parse (if parsing again)
    structs_.clear();
    resources_.clear();
    
    vs_material_uniform_buffer_.reset();
    fs_material_uniform_buffer_.reset();
    gs_material_uniform_buffer_.reset();
    
    const char* token;
    for (auto& [domain, string] : shader_source_code_map_) {
      auto& shader_source_code = string;
      
      // -------------------
      // Parsing Structures
      // -------------------
      const char* vstr = shader_source_code.c_str();
      while ((token = StringUtils::FindToken(vstr, "struct"))) {
        ParseUniformStruct(StringUtils::GetBlock(token, &vstr),
                           shader_utils::GlDomainToShaderDomain((GLint)domain));
      }
      
      // -------------------
      // Parsing uniforms
      // -------------------
      vstr = shader_source_code.c_str();
      IK_CORE_DEBUG("  Parsing the '{0}' shader to extracts all the Uniforms for "
                    "'{1}' Shader",
                    name_, shader_utils::ShaderNameFromType(domain));

      IK_CORE_DEBUG("    Parsing the Uniforms: ");
      while ((token = StringUtils::FindToken(vstr, "uniform"))) {
        ParseUniform(StringUtils::GetStatement(token, &vstr),
                     shader_utils::GlDomainToShaderDomain((GLint)domain));
      }
    } // for (auto& [domain, string] : shader_source_code_map_)
  }
  
  void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain) {
    if (structs_.size() == 0) {
      IK_CORE_DEBUG("  Parsing the '{0}' shader to extracts all the Structures for '{1}' Shader",
                    name_, shader_utils::ShaderNameFromType(domain));
    }
    
    // get each word from the block and store them in vector
    std::vector<std::string> tokens = StringUtils::Tokenize(block);
    uint32_t index = 1; // 0 is for keyword "struct"
    
    // get the name of structure
    std::string struct_name = tokens[index++];
    ShaderStruct* uniform_struct = new ShaderStruct(struct_name);
    index++;  // 1 is for Name (stored)

    IK_CORE_DEBUG("    struct {0} ", struct_name);
    IK_CORE_DEBUG("    {");
    
    // Parse the strcuture
    while (index < tokens.size()) {
      if (tokens[index] == "}")
        break;

      std::string field_type = tokens[index++]; // Type of element
      std::string field_name = tokens[index++]; // Name of element

      // Strip ';' from name if present
      if (const char* s = strstr(field_name.c_str(), ";")) {
        size_t size_ot_field_name = (size_t)(s - field_name.c_str());
        field_name = std::string(field_name.c_str(), size_ot_field_name);
      }

      // Check is it array if yes the extract count
      uint32_t count = 1;
      const char* name_str = field_name.c_str();
      if (const char* count_with_brackets = strstr(name_str, "[")) {
        auto name_without_count = std::string(name_str, (size_t)(count_with_brackets - name_str));
        const char* end = strstr(name_str, "]");
        std::string count_with_last_bracket(count_with_brackets + 1,
                                       (size_t)(end - count_with_brackets));
        
        count = (uint32_t)atoi(count_with_last_bracket.c_str());
        field_name = name_without_count;
      } // if (const char* count_with_brackets = strstr(name_str, "["))

      // Stores the content of structure in struct
      ShaderUniformDeclaration*
      field = new OpenGLShaderUniformDeclaration(domain,
                                                 OpenGLShaderUniformDeclaration::StringToType(field_type),
                                                 field_name,
                                                 count);
      uniform_struct->AddField(field);
    }
    
    IK_CORE_DEBUG("    }");
    structs_.emplace_back(uniform_struct);
  }
  
  void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain) {
    std::vector<std::string> tokens = StringUtils::Tokenize(statement);
    uint32_t index = 1; // 0th is for keyword unifrom
    
    std::string field_type = tokens[index++];
    std::string field_name = tokens[index++];

    // Strip ; from name if present
    if (const char* s = strstr(field_name.c_str(), ";"))
      field_name = std::string(field_name.c_str(), (size_t)(s - field_name.c_str()));
    
    std::string name_with_size(field_name);
    
    // Check is it array if yes the extract count
    uint32_t count = 1;
    const char* name_str = name_with_size.c_str();
    if (const char* count_with_brackets = strstr(name_str, "[")) {
      auto name_without_count = std::string(name_str, (size_t)(count_with_brackets - name_str));
      const char* end = strstr(name_str, "]");
      std::string count_with_last_bracket(count_with_brackets + 1,
                                     (size_t)(end - count_with_brackets));
      
      count = (uint32_t)atoi(count_with_last_bracket.c_str());
      field_name = name_without_count;
    } // if (const char* count_with_brackets = strstr(name_str, "["))

    // Store the resources uniform inside shader
    if (shader_utils::IsTypeStringResource(field_type)) {
      // Resources like Sampler 2D
      ShaderResourceDeclaration*
      declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(field_type),
                                                        field_name,
                                                        count);
      resources_.push_back(declaration);
    }
    else { //  if field is not of type resources like sampler
           // Resources fundamental and structures
      OpenGLShaderUniformDeclaration::Type type = OpenGLShaderUniformDeclaration::StringToType(field_type);
      OpenGLShaderUniformDeclaration* declaration = nullptr;
      
      if (type == OpenGLShaderUniformDeclaration::Type::None) {
        // Find struct
        // NOTE: in this case field type is the name of struct as we write like
        // this " uniform < field type     >  < field name   > "
        //      " uniform < name of struct >  < uniform name > "
        ShaderStruct* structure = FindStruct(field_type);
        IK_CORE_ASSERT(structure, "");
        declaration = new OpenGLShaderUniformDeclaration(domain,
                                                         structure,
                                                         field_name,
                                                         count);
      } // if (type == OpenGLShaderUniformDeclaration::Type::kStruct)
      else {
        declaration = new OpenGLShaderUniformDeclaration(domain,
                                                         type,
                                                         field_name,
                                                         count);
      }
      
      // Store all the uniforms in buffers
      if (domain == ShaderDomain::Vertex) {
        if (!vs_material_uniform_buffer_)
          vs_material_uniform_buffer_.reset(new OpenGLShaderUniformBufferDeclaration("", domain));
        
        vs_material_uniform_buffer_->PushUniform(declaration);
      }
      else if (domain == ShaderDomain::Fragment) {
        if (!fs_material_uniform_buffer_)
          fs_material_uniform_buffer_.reset(new OpenGLShaderUniformBufferDeclaration("", domain));
        
        fs_material_uniform_buffer_->PushUniform(declaration);
      }
      else if (domain == ShaderDomain::Geometry) {
        if (!gs_material_uniform_buffer_)
          gs_material_uniform_buffer_.reset(new OpenGLShaderUniformBufferDeclaration("", domain));
        
        gs_material_uniform_buffer_->PushUniform(declaration);
      }
    } // else : if (shader_utils::IsTypeStringResource(field_type))
  }
  
  ShaderStruct* OpenGLShader::FindStruct(const std::string& name) {
    for (ShaderStruct* s : structs_)
      if (s->GetName() == name)
        return s;
    return nullptr;
  }
  
  int32_t OpenGLShader::GetUniformLocation(const std::string& name) {
    if (location_map_.find(name) != location_map_.end())
      return location_map_[name];
    
    int32_t location = glGetUniformLocation(renderer_id_, name.c_str());
    if (-1 == location)
      IK_CORE_WARN("Warning: uniform '{0}' doesnt exist", name);
    
    location_map_[name] = location;
    return location;
  }
  
  void OpenGLShader::ResolveUniforms() {
    IK_CORE_DEBUG("  Resolving Uniform locations for Shader '{0}'", name_);
    
    // -------------------------------------------
    // Uniform samplers for textures, cubemaps etc
    // -------------------------------------------
    IK_CORE_DEBUG("    Resolving Uniforms for Samplers...");
    // Setting location of sampler uniform
    uint32_t sampler = 0;
    for (size_t i = 0; i < resources_.size(); i++) {
      OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)resources_[i];
      int32_t location = GetUniformLocation(resource->name_);
      
      // For single samplers
      if (resource->GetCount() == 1) {
        resource->register_ = sampler;
        if (location != -1) {
          IK_CORE_DEBUG("      Location : {0} for {1}[{2}]",
                        sampler, resource->name_, resource->GetCount());
          SetUniformInt1(resource->name_, (int32_t)sampler);
        }
        sampler++;
      }
      // For arrya of samplers
      else if (resource->GetCount() > 1) {
        resource->register_ = 0;
        
        uint32_t count = resource->GetCount();
        int32_t* samplers = new int32_t[count];
        
        for (uint32_t s = 0; s < count; s++)
          samplers[s] = (int32_t)s;
        IK_CORE_DEBUG("      Location : {0} to {1} for {2}[{3}]", 0,
                      count, resource->GetName(), resource->GetCount());
        SetIntArray(resource->GetName(), samplers, count);
        delete[] samplers;
      }
    } // for (size_t i = 0; i < resources_.size(); i++)
    
    // -----------------------------------------------
    // Unifrom resources of structors or fundamentals
    // -----------------------------------------------
    std::shared_ptr<OpenGLShaderUniformBufferDeclaration> decls[3] = {
      vs_material_uniform_buffer_,
      fs_material_uniform_buffer_,
      gs_material_uniform_buffer_
    };
    
    for (uint8_t shaderIdx = 0; shaderIdx < MaxShaderSupported; shaderIdx++) {
      auto decl = decls[shaderIdx];
      if (!decl) {
        continue;
      }

      IK_CORE_DEBUG("    Resolving Uniforms for Datatypes of '{0}' Shader...",
                    shader_utils::ShaderNameFromInternalType((ShaderDomain)(shaderIdx + 1)));

      const std::vector<ShaderUniformDeclaration*>& uniforms = decl->GetUniformDeclarations();
      for (size_t j = 0; j < uniforms.size(); j++) {
        OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
        
        // Uniform Structures
        if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct) {
          const ShaderStruct& s = uniform->GetShaderUniformStruct();
          const auto& fields = s.GetFields();
          
          // If Array of Structure
          if (uniform->GetCount() > 1) {
            for (size_t l = 0; l < uniform->GetCount(); l++) {
              for (size_t k = 0; k < fields.size(); k++) {
                OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
                std::string uniform_name = uniform->name_ + "[" + std::to_string(l) + "]." + field->name_;
                uint32_t location = (uint32_t)GetUniformLocation(uniform_name);
                field->location_.emplace_back(location);
                IK_CORE_DEBUG("      Location : {0} for {1}[{3}].{2}",
                              location, s.GetName(), field->GetName(), l);
              } // for (size_t k = 0; k < fields.size(); k++)
            } // for (size_t l = 0; l < uniform->GetCount(); l++)
          } // if (uniform->GetCount() > 1)
          
          // Single struct uniform
          else {
            for (size_t k = 0; k < fields.size(); k++) {
              OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
              uint32_t location = (uint32_t)GetUniformLocation(uniform->name_ + "." + field->name_);
              field->location_.emplace_back(location);
              IK_CORE_DEBUG("      Location : {0} for {1}.{2} [{3}]",
                            location, s.GetName(), field->GetName(), field->GetCount());
            }
          } // else : if (uniform->GetCount() > 1)
        } // if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::kStruct)
        
        // Fundamental uniforms
        else {
          uint32_t location = (uint32_t)GetUniformLocation(uniform->name_);
          uniform->location_.emplace_back(location);
          IK_CORE_DEBUG("      Location : {0} for {1}[{2}]",
                        location, uniform->GetName(), uniform->GetCount());
        }
      } // for (size_t j = 0; j < uniforms.size(); j++)
    } // for (uint8_t shaderIdx = 0; shaderIdx < MaxShaderSupported; shaderIdx++)
  }
  
  void OpenGLShader::SetVSMaterialUniformBuffer(const Buffer& buffer) {
    glUseProgram(renderer_id_);
    ResolveAndSetUniforms(vs_material_uniform_buffer_, buffer);
  }
  
  void OpenGLShader::SetFSMaterialUniformBuffer(const Buffer& buffer) {
    glUseProgram(renderer_id_);
    ResolveAndSetUniforms(fs_material_uniform_buffer_, buffer);
  }
  
  void OpenGLShader::SetGSMaterialUniformBuffer(const Buffer& buffer) {
    glUseProgram(renderer_id_);
    ResolveAndSetUniforms(gs_material_uniform_buffer_, buffer);
  }
  
  void OpenGLShader::ResolveAndSetUniforms(const std::shared_ptr<OpenGLShaderUniformBufferDeclaration>& decl,
                                           const Buffer& buffer) {
    const std::vector<ShaderUniformDeclaration*>& uniforms = decl->GetUniformDeclarations();
    for (size_t i = 0; i < uniforms.size(); i++) {
      OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
      if (uniform->IsArray())
        ResolveAndSetUniformArray(uniform, buffer);
      else
        ResolveAndSetUniform(uniform, buffer);
    }
  }
  
  void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform,
                                          const Buffer& buffer) {
    // TODO: Add Check of location later
    uint32_t offset = uniform->GetOffset();
    switch (uniform->GetType()) {
      case OpenGLShaderUniformDeclaration::Type::Float32:
        UploadUniformFloat1(uniform->GetLocation(), *(float*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Int32:
      case OpenGLShaderUniformDeclaration::Type::Bool:
        UploadUniformInt1(uniform->GetLocation(), *(int32_t*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec2:
        UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec3:
        UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec4:
        UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat3:
        UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat4:
        UploadUniformMat4(uniform->GetLocation(), *(glm::mat4*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Struct:
        UploadUniformStruct(uniform, buffer.data, offset);
        break;
      default:
        IK_CORE_ASSERT(false, "Unknown uniform type!");
    }
  }
  
  void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform,
                                               const Buffer& buffer) {
    // TODO: Add Check of location later
    uint32_t offset = uniform->GetOffset();
    switch (uniform->GetType()) {
      case OpenGLShaderUniformDeclaration::Type::Float32:
        UploadUniformFloat1(uniform->GetLocation(),
                            *(float*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Int32:
      case OpenGLShaderUniformDeclaration::Type::Bool:
        UploadUniformInt1(uniform->GetLocation(),
                          *(int32_t*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec2:
        UploadUniformFloat2(uniform->GetLocation(),
                            *(glm::vec2*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec3:
        UploadUniformFloat3(uniform->GetLocation(),
                            *(glm::vec3*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec4:
        UploadUniformFloat4(uniform->GetLocation(),
                            *(glm::vec4*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat3:
        UploadUniformMat3(uniform->GetLocation(),
                          *(glm::mat3*)&buffer.data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat4:
        UploadUniformMat4Array((uint32_t)uniform->GetLocation(),
                               *(glm::mat4*)&buffer.data[offset],
                               uniform->GetCount());
        break;
      case OpenGLShaderUniformDeclaration::Type::Struct:
        UploadUniformStruct(uniform, buffer.data, offset);
        break;
      default:
        IK_CORE_ASSERT(false, "Unknown uniform type!");
    }
  }
  
  void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field,
                                               std::byte* data,
                                               int32_t offset,
                                               uint8_t idx) {
    int32_t location = field.location_[idx];
    if (location == -1)
      return;
    
    switch (field.GetType()) {
      case OpenGLShaderUniformDeclaration::Type::Float32:
        UploadUniformFloat1(location, *(float*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Int32:
      case OpenGLShaderUniformDeclaration::Type::Bool:
        UploadUniformInt1(location, *(int32_t*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec2:
        UploadUniformFloat2(location, *(glm::vec2*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec3:
        UploadUniformFloat3(location, *(glm::vec3*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Vec4:
        UploadUniformFloat4(location, *(glm::vec4*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat3:
        UploadUniformMat3(location, *(glm::mat3*)&data[offset]);
        break;
      case OpenGLShaderUniformDeclaration::Type::Mat4:
        UploadUniformMat4(location, *(glm::mat4*)&data[offset]);
        break;
      default:
        IK_CORE_ASSERT(false, "Unknown uniform type!");
    }
  }
  
  void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform,
                                         std::byte* buffer,
                                         uint32_t offset) {
    const ShaderStruct& s = uniform->GetShaderUniformStruct();
    const auto& fields = s.GetFields();
    
    for (uint8_t i = 0; i < uniform->GetCount(); i++) {
      for (size_t k = 0; k < fields.size(); k++) {
        OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
        ResolveAndSetUniformField(*field, buffer, (int32_t)offset, i);
        offset += field->size_;
      }
    }
  }
  
  void OpenGLShader::Bind() const { glUseProgram(renderer_id_); }
  void OpenGLShader::Unbind() const { glUseProgram(0); };
  
  const std::string& OpenGLShader::GetName() const { return name_; }
  const std::string& OpenGLShader::GetFilePath() const { return asset_path_; }
  RendererID OpenGLShader::GetRendererID() const { return renderer_id_; }

  
  // --------------------------------------------------------------------------
  // Uniforms with name
  // --------------------------------------------------------------------------
  /// No need to Submit these API in Renderer command queue as they are taken
  /// care in Resolving Uniform API
  void OpenGLShader::SetUniformInt1(const std::string& name,
                                    int32_t value) {
    glUseProgram(renderer_id_);
    glUniform1i(GetUniformLocation(name),
                value);
  }
  
  /// No need to Submit these API in Renderer command queue as they are taken
  /// care in Resolving Uniform API
  void OpenGLShader::SetIntArray(const std::string& name,
                                 int32_t* values,
                                 uint32_t count) {
    glUseProgram(renderer_id_);
    int32_t* textureArraySlotData = new int32_t[count];
    memcpy(textureArraySlotData, values, count * sizeof(int32_t));
    
    glUniform1iv(GetUniformLocation(name),
                 (GLsizei)count,
                 textureArraySlotData);
    delete[] textureArraySlotData;
  }
  
  void OpenGLShader::SetUniformMat4Array(const std::string& name,
                                         const glm::mat4& values,
                                         uint32_t count) {
    glUniformMatrix4fv(GetUniformLocation(name),
                       (GLsizei)count,
                       GL_FALSE,
                       glm::value_ptr(values));
  }
  
  void OpenGLShader::SetUniformMat4(const std::string& name,
                                    const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name),
                       1,
                       GL_FALSE,
                       glm::value_ptr(value));
  }
  
  void OpenGLShader::SetUniformMat3(const std::string& name,
                                    const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name),
                       1,
                       GL_FALSE,
                       glm::value_ptr(value));
  }
  
  void OpenGLShader::SetUniformFloat1(const std::string& name,
                                      float value) {
    glUniform1f(GetUniformLocation(name),
                value);
  }
  
  void OpenGLShader::SetUniformFloat2(const std::string& name,
                                      const glm::vec2& value) {
    glUniform2f(GetUniformLocation(name),
                value.x,
                value.y);
  }
  
  void OpenGLShader::SetUniformFloat3(const  std::string& name,
                                      const glm::vec3& value) {
    glUniform3f(GetUniformLocation(name),
                value.x,
                value.y,
                value.z);
  }
  
  void OpenGLShader::SetUniformFloat4(const std::string& name,
                                      const glm::vec4& value) {
    glUniform4f(GetUniformLocation(name),
                value.x,
                value.y,
                value.z,
                value.w);
  }
  
  // --------------------------------------------------------------------------
  // Uniforms with location
  // --------------------------------------------------------------------------
  void OpenGLShader::UploadUniformInt1(int32_t location, int32_t value) {
    glUniform1i(location, value);
  }
  
  void OpenGLShader::UploadUniformMat4(int32_t location, const glm::mat4& value) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }
  
  void OpenGLShader::UploadUniformMat3(int32_t location, const glm::mat3& value) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }
  
  void OpenGLShader::UploadUniformFloat1(int32_t location, float value) {
    glUniform1f(location, value);
  }
  
  void OpenGLShader::UploadUniformFloat2(int32_t location, const glm::vec2& value) {
    glUniform2f(location, value.x, value.y);
  }
  
  void OpenGLShader::UploadUniformFloat3(int32_t location, const glm::vec3& value) {
    glUniform3f(location, value.x, value.y, value.z);
  }
  
  void OpenGLShader::UploadUniformFloat4(int32_t location, const glm::vec4& value) {
    glUniform4f(location, value.x, value.y, value.z, value.w);
  }
  
  void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count) {
    glUniformMatrix4fv((GLint)location, (GLsizei)count, GL_FALSE, glm::value_ptr(values));
  }
  
  bool OpenGLShader::HasVSMaterialUniformBuffer() const {
    return (bool)vs_material_uniform_buffer_;
  }
  bool OpenGLShader::HasFSMaterialUniformBuffer() const {
    return (bool)fs_material_uniform_buffer_;
  }
  bool OpenGLShader::HasGSMaterialUniformBuffer() const {
    return (bool)gs_material_uniform_buffer_;
  }
  
  const ShaderUniformBufferDeclaration& OpenGLShader::GetVSMaterialUniformBuffer() const {
    return *vs_material_uniform_buffer_;
  }
  const ShaderUniformBufferDeclaration& OpenGLShader::GetFSMaterialUniformBuffer() const {
    return *fs_material_uniform_buffer_;
  }
  const ShaderUniformBufferDeclaration& OpenGLShader::GetGSMaterialUniformBuffer() const {
    return *gs_material_uniform_buffer_;
  }
  
  const std::vector<ShaderResourceDeclaration*>& OpenGLShader::GetResources() const {
    return resources_;
  }

}
