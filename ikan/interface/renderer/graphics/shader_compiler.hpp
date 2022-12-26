//
//  shader_compiler.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  /// This is interface class to complile graphics shader
  class Shader {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// This is the default virtual destructor
    virtual ~Shader() noexcept = default;
    
    // -------------
    // Getters
    // -------------
    /// This function returns the Name of Shader
    virtual const std::string& GetName() const = 0;
    /// This function returns the File Path of Shader
    virtual const std::string& GetFilePath() const = 0;
    /// This function returns the Renderer ID of Shader
    virtual RendererID GetRendererID() const = 0;
    
    // ---------------
    // Static Create
    // ---------------
    /// This static function creates Shader Instance based on the Suported API
    /// - Parameter path: Absolute Path of shader
    static std::shared_ptr<Shader> Create(const std::string& file_path);
  };
  
}
