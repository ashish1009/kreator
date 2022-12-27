//
//  open_gl_texture.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  // -------------------------------------------------------------------------
  // Texture Class for Open GL
  // -------------------------------------------------------------------------
  /// Implementation for Open GL Texture class
  class OpenGLTexture : public Texture {
  public:
    // ------------------------------
    // Constructors and Destructors
    // ------------------------------
    /// This constructor creates the texture using image file path
    /// - Parameters:
    ///   - file_path: Texture image file path (absolute)
    ///   - min_linear: min linear filter
    ///   - mag_linear: mag linear filter
    OpenGLTexture(const std::string& file_path, bool min_linear, bool mag_linear);
    
    /// Default destructor that delete the texture
    virtual ~OpenGLTexture() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLTexture);
    
  public:
    RendererID renderer_id_ = 0;
    
    bool uploaded_ = false;

    int32_t width_ = 0, height_ = 0;
    int32_t channel_ = 0;
    uint32_t size_ = 0;
    uint32_t internal_format_ = 0, data_format_ = 0;
    
    void* texture_data_;
    std::string file_path_ = "", name_ = "";
  };
  
}
