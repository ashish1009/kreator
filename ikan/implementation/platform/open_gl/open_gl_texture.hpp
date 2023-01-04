//
//  open_gl_texture.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "renderer/graphics/texture.hpp"
#include <glad/glad.h>

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
  
  // -------------------------------------------------------------------------
  // Char Texture Class for Open GL
  // -------------------------------------------------------------------------
  /// Implementaiton for Open GL Char Texture
  class OpenGLCharTexture : public CharTexture {
  public:
    // ------------------------------
    // Constructors and Destructors
    // ------------------------------
    /// This constructor creates the Open GL Texture for rendering char
    /// - Parameters:
    ///   - face: face of char
    ///   - size: size of char
    ///   - bearing: bearing
    ///   - advance: advance
    ///   - char_val: character value
    OpenGLCharTexture(const FT_Face& face,
                      const glm::ivec2& size,
                      const glm::ivec2& bearing,
                      uint32_t advance,
                      [[maybe_unused]] char char_val);
    /// This is the default Open GL Char Texture destructor
    virtual ~OpenGLCharTexture();
    
    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLCharTexture);
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the texture
    void Bind(uint32_t slot) const override;
    /// This function unbinds the texture
    void Unbind() const  override;

    // ----------
    // Getters
    // ----------
    /// This function returns the Renderer ID
    RendererID GetRendererID() const override;
    /// This function returns width of texture
    uint32_t GetWidth() const override;
    /// This function returns height of texture
    uint32_t GetHeight() const override;
    /// This function returns the size
    glm::ivec2 GetSize() const override;
    /// This function returns the bearing
    glm::ivec2 GetBearing() const override;
    /// This function returns the advance
    uint32_t GetAdvance() const override;
    
  private:
    RendererID renderer_id_;
    glm::ivec2 size_;
    glm::ivec2 bearing_;
    uint32_t advance_ = 0;
    uint32_t data_size_ = 0;
    uint32_t width_ = 0, height_ = 0;
  };
  
  class OpenGLImage : public Image {
  public:
    // ------------------------------
    // Constructors and Destructors
    // ------------------------------
    /// This Constructor create the empty Open GL texture with user data
    /// - Parameters:
    ///   - width: widht of texture
    ///   - height: height of texture
    ///   - format: format of texture
    ///   - data: data to load in texture
    OpenGLImage(uint32_t width,
                uint32_t height,
                TextureFormat format,
                void* data);
    
    /// Default destructor that delete the texture
    virtual ~OpenGLImage() noexcept;

    DELETE_COPY_MOVE_CONSTRUCTORS(OpenGLImage);

    // -------------
    // Fundamentals
    // -------------
    /// This function loads the data in GPU
    /// - Parameter data: data to be loaded
    void SetData(void* data) override;
    
    // ----------
    // Getters
    // ----------
    /// This function returns the Renderer ID
    RendererID GetRendererID() const override;
    /// This function returns width of texture
    uint32_t GetWidth() const override;
    /// This function returns height of texture
    uint32_t GetHeight() const override;
    /// This function returns the size
    uint32_t GetSize() const override;
    /// This function loads the data in GPU
    /// - Parameters:
    ///   - width: width of image
    ///   - height: height of image
    void Resize(uint32_t width, uint32_t height) override;
    
  private:
    // Member Functions
    void Invalidate();

    RendererID renderer_id_ = 0;
    uint32_t internal_format_ = 0, data_format_ = 0;
    int32_t width_ = 0, height_ = 0;
    uint32_t size_ = 0;
    void* image_data_ = nullptr;
  };
  
  
  namespace texture_utils {
    
#ifdef IK_DEBUG_FEATURE
    
    /// This function returns the Format name from Enum
    /// - Parameter format: enum taken as uint (enum in Glad)
    std::string GetFormatNameFromEnum(uint32_t format);

#endif

    /// This function returns the Open GL Format type from i kan type
    /// - Parameter format: i kan type
    GLint ikanFormatToOpenGLFormat(TextureFormat format);
    /// This function returns the texture fype from internal format
    /// - Parameter format_tyoe: format type
    GLint GetTextureType(GLint format_tyoe);

  } 
  
}
