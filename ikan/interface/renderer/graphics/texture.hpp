//
//  texture.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

#include "renderer/utils/renderer.hpp"

namespace ikan {
  
  /// Interface class for Storing Renderer Texture data. Implementation is depending on the Supported Renerer API.
  class Texture {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// Default virtual destructor
    virtual ~Texture() noexcept = default;
    
    // -----------------
    // Static Function
    // -----------------
    /// This static functions creates the Texture from image file
    /// - Parameters:
    ///   - file_path: path of texture file
    ///   - min_linear: min linear flag
    ///   - mag_linear: max linear flag
    static std::shared_ptr<Texture> Create(const std::string& file_path,
                                           bool min_linear = true,
                                           bool mag_linear = true);
    /// This static functions creates Emptry Texture with user Defined Data of
    /// size height and Width
    /// - Parameters:
    ///   - width: Width of Empty
    ///   - height Height of Empty Texture
    ///   - data: Data to be stored in Empty Texture
    ///   - size: Size of type of data stored in Texture
    static std::shared_ptr<Texture> Create(uint32_t width,
                                           uint32_t height,
                                           void* data,
                                           uint32_t size);
  };
  
}
