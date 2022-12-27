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
    /// This Constructor create the empty Open GL texture with user data
    /// - Parameters:
    ///   - width: widht of texture
    ///   - height: height of texture
    ///   - data: user data to fill in texture
    ///   - size: size of texture
    OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size);

    /// Default destructor that delete the texture
    virtual ~OpenGLTexture() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLTexture);
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the texture at slot
    /// - Parameter slot: shader slot where this texture to be binded
    void Bind(uint32_t slot = 0) const override;
    /// This function unbinds the texture
    void Unbind() const override;

    // ----------
    // Getters
    // ----------
    /// This function returns width of texture
    uint32_t GetWidth() const override;
    /// This function returns height of texture
    uint32_t GetHeight() const override;
    /// This function returns renderer ID of texture
    RendererID GetRendererID() const override;
    /// This function returns file path of texture
    const std::string& GetfilePath() const override;
    /// This function returns name of texture
    const std::string& GetName() const override;
        
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
