//
//  shader_uniform.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "shader_uniform.hpp"

namespace ikan {
  
  ShaderStruct::ShaderStruct(const std::string& name)
  : name_(name), size_(0), offset_(0) {
    IK_CORE_TRACE(LogModule::Shader, "  Creating Open GL Structure {0}...", name_.c_str());
  }
  
  ShaderStruct::~ShaderStruct() {
    IK_CORE_WARN(LogModule::Shader, "  Destroying Open GL Structure {0} !!!", name_.c_str());
    
    for (auto& field : fields_)
      delete field;
  }
  
  void ShaderStruct::SetOffset(uint32_t offset) { offset_ = offset; }
  
  uint32_t ShaderStruct::GetSize() const { return size_; }
  uint32_t ShaderStruct::GetOffset() const { return offset_; }
  
  const std::string& ShaderStruct::GetName() const { return name_; }
  const std::vector<ShaderUniformDeclaration*>& ShaderStruct::GetFields() const {
    return fields_;
  }
  
  void ShaderStruct::AddField(ShaderUniformDeclaration* field) {
    size_ += field->GetSize();
    uint32_t offset = 0;
    if (fields_.size()) {
      ShaderUniformDeclaration* previous = fields_.back();
      offset = previous->GetOffset() + previous->GetSize();
    }
    field->SetOffset(offset);
    fields_.push_back(field);
  }
  
} // namespace ikan
