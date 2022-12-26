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
  }
  
  OpenGLShader::~OpenGLShader() noexcept {
    IK_CORE_WARN("Destroying Open GL Shader ...");
    IK_CORE_WARN("  Renderer ID | {0} ", renderer_id_);
    IK_CORE_WARN("  Name        | {0} ", name_);
    IK_CORE_WARN("  File Path   | {0} ", asset_path_);
    
    for (auto& structure : structs_)
      delete structure;
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
      
//      // -------------------
//      // Parsing uniforms
//      // -------------------
//      vstr = shader_source_code.c_str();
//      IK_CORE_DEBUG("  Parsing the '{0}' shader to extracts all the Uniforms for "
//                    "'{1}' Shader",
//                    name_, shader_utils::ShaderNameFromType(domain));
//
//      IK_CORE_DEBUG("    Parsing the Uniforms: ");
//      while ((token = StringUtils::FindToken(vstr, "uniform"))) {
//        ParseUniform(StringUtils::GetStatement(token, &vstr),
//                     shader_utils::GlDomainToShaderDomain((GLint)domain));
//      }
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
   
  }

  const std::string& OpenGLShader::GetName() const { return name_; }
  const std::string& OpenGLShader::GetFilePath() const { return asset_path_; }
  RendererID OpenGLShader::GetRendererID() const { return renderer_id_; }

}
