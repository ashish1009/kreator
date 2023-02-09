//
//  texture.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

#include "renderer/utils/renderer.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ikan {
  
  enum class TextureFormat  {
    None = 0, RGBA
  };
  
  class Texture;
  class SubTexture;
  /// This structure holds the texture component with use flag
  struct TextureComponent {
    bool use = false;
    bool use_sprite = false;

    std::shared_ptr<Texture> component = nullptr;
    std::shared_ptr<SubTexture> sprite = nullptr;

    float tiling_factor = 1.0f;
    
    // default constructor and destructors
    TextureComponent() = default;
    ~TextureComponent() noexcept = default;
    
    // Copy and Move Constructor and operator =
    DEFINE_COPY_MOVE_CONSTRUCTORS(TextureComponent)
    
    // Parameter constructor
    TextureComponent(const std::shared_ptr<Texture>& comp, bool use = true);
    void LoadTexture(const std::shared_ptr<Texture> other_component);
  };

  /// Interface class for Storing Renderer Texture data. Implementation is depending on the Supported Renerer API.
  class Texture {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// Default virtual destructor
    virtual ~Texture() = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the Current Texture to a slot of shader
    /// - Parameter slot: Slot of shader
    virtual void Bind(uint32_t slot = 0) const = 0;
    /// This function unbinds the Current Texture from shader slot
    virtual void Unbind() const = 0;

    // -------------
    // Getters
    // -------------
    /// This function returns the Renderer ID of Texture
    [[nodiscard]] virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;
    /// This function returns the File Path of Texture NOTE: Return "" for white texture
    [[nodiscard]] virtual const std::string& GetfilePath() const = 0;
    /// This function returns name of texture
    [[nodiscard]] virtual const std::string& GetName() const = 0;

    // -----------------
    // Static Function
    // -----------------
    /// This static functions creates the Texture from image file
    /// - Parameters:
    ///   - file_path: path of texture file
    ///   - min_linear: min linear flag
    ///   - mag_linear: max linear flag
    [[nodiscard]] static std::shared_ptr<Texture> Create(const std::string& file_path,
                                                         bool min_linear = true,
                                                         bool mag_linear = true);
    /// This static functions creates Emptry Texture with user Defined Data of
    /// size height and Width
    /// - Parameters:
    ///   - width: Width of Empty
    ///   - height Height of Empty Texture
    ///   - data: Data to be stored in Empty Texture
    ///   - size: Size of type of data stored in Texture
    [[nodiscard]] static std::shared_ptr<Texture> Create(uint32_t width,
                                                         uint32_t height,
                                                         void* data,
                                                         uint32_t size);
  };
  
  /// Wrepper class to load texture and render as sprite
  class SubTexture {
  public:
    /// This Funtion create the subtexture instance with texture reference pointer and following params
    /// - Parameters:
    ///   - sprite_image: sprite_image
    ///   - min: min bound of sub texture
    ///   - max: mxn bound of sub texture
    ///   - coords: coordinate sof sub image from sprite image with reference to the cell Size.
    ///              e.g {1, 2} -> from top left corner of image use the block of size 'cell size'
    ///             from 16th pixel frol eft and 32nd pixled from top (using 16 assuming cell size is {16, 17})
    ///   - sprite_size: number of blocks to be taken at one time from sprite
    ///   - cell_size: size of block in pixel to be used for 1 block
    SubTexture(const std::shared_ptr<Texture>& sprite_image,
               const glm::vec2& min,
               const glm::vec2& max,
               const glm::vec2& coords = {0.0f, 0.0f},
               const glm::vec2& sprite_size = {1.0f, 1.0f},
               const glm::vec2& cell_size = {16.0f, 16.0f});
    
    /// Default destructor
    ~SubTexture() = default;
    
    // -----------------
    // Getters
    // -----------------
    /// This function returns the texture reference of sprite
    std::shared_ptr<Texture> GetSpriteImage();
    /// This function returns Texture coordinates of Subtexture
    const glm::vec2* GetTexCoord() const;
    /// This function returns Sprite size of Subtexture
    glm::vec2& GetSpriteSize();
    /// This function returns Cell Size of Subtexture
    glm::vec2& GetCellSize();
    /// This function returnsCoordinates of Subtexture
    glm::vec2& GetCoords();

    /// This Funtion create the subtexture instance with texture reference pointer and following params
    /// - Parameters:
    ///   - sprite_image: sprite_image
    ///   - coords: coordinate sof sub image from sprite image with reference to the cell Size.
    ///              e.g {1, 2} -> from top left corner of image use the block of size 'cell size'
    ///             from 16th pixel frol eft and 32nd pixled from top (using 16 assuming cell size is {16, 17})
    ///   - sprite_size: number of blocks to be taken at one time from sprite
    ///   - cell_size: size of block in pixel to be used for 1 block
    static std::shared_ptr<SubTexture> CreateFromCoords(const std::shared_ptr<Texture>& sprite_image,
                                                        const glm::vec2& coords,
                                                        const glm::vec2& sprite_size = {1.0f, 1.0f},
                                                        const glm::vec2& cell_size = {16.0f, 16.0f});

  private:
    std::shared_ptr<Texture> sprite_image_;
    glm::vec2 texture_coord_[4];
    glm::vec2 sprite_size_;
    glm::vec2 cell_size_;
    glm::vec2 coords_;
  };
  
  /// This class is the Interface for Storing Renderer Char Texture data. Implementation is depending on the Supported Renerer API.
  class CharTexture {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// This is the defualt destructor of char texture
    virtual ~CharTexture() = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This function binds the Current Texture to a slot of shader
    /// - Parameter slot: Slot of shader
    virtual void Bind(uint32_t slot = 0) const = 0;
    /// This function unbinds the Current Texture from shader slot
    virtual void Unbind() const = 0;

    // -------------
    // Getters
    // -------------
    /// This function returns the Renderer ID of Texture
    [[nodiscard]] virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;
    /// This function returns the Size of Freetpe face
    [[nodiscard]] virtual glm::ivec2 GetSize() const = 0;
    /// This function returns the Bearing of Freetpe face
    [[nodiscard]] virtual glm::ivec2 GetBearing() const = 0;
    /// This function returns the Advance of Freetpe face
    [[nodiscard]] virtual uint32_t GetAdvance() const = 0;
    
    // -----------------
    // Static Function
    // -----------------
    /// This function creates Emptry Texture with user Defined Data of size height and Width
    /// - Parameters:
    ///   - face: face of char
    ///   - size: size of char
    ///   - bearing: bearing
    ///   - advance: advance
    ///   - char_val: character value
    [[nodiscard]] static std::shared_ptr<CharTexture> Create(const FT_Face& face,
                                               const glm::ivec2& size,
                                               const glm::ivec2& bearing,
                                               uint32_t advance,
                                               [[maybe_unused]] char char_val);
  };
  
  class Image {
  public:
    // -------------
    // Destrcutor
    // -------------
    /// Default virtual destructor
    virtual ~Image() = default;

    // -------------
    // Fundamentals
    // -------------
    /// This function loads the data in GPU
    /// - Parameter data: data to be loaded
    virtual void SetData(void* data) = 0;
    /// This function loads the data in GPU
    /// - Parameters:
    ///   - width: width of image
    ///   - height: height of image
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
    // -------------
    // Getters
    // -------------
    /// This function returns the Renderer ID of Texture
    [[nodiscard]] virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;
    /// This function returns the Size of Freetpe face
    [[nodiscard]] virtual uint32_t GetSize() const = 0;

    /// This static functions creates the Texture without data of size
    /// - Parameters:
    ///   - width: width of image
    ///   - height: height of image
    ///   - format: image format
    ///   - data: data of image
    [[nodiscard]] static std::shared_ptr<Image> Create(uint32_t width,
                                                       uint32_t height,
                                                       TextureFormat format,
                                                       void* data = nullptr);
  };

  /// This class stores the compiled Texture in library
  class TextureLibrary {
  private:
    // -----------
    // Functions
    // -----------
    /// This function returns the Ref type of ikan::Texture. It creates a new if not present in the map
    /// - Parameters:
    ///   - path: path of textre
    ///   - min_linear: min linear flag
    ///   - mag_linear: max linear flag
    [[nodiscard]] static std::shared_ptr<Texture> GetTexture(const std::string& path,
                                                             bool min_linear = true,
                                                             bool mag_linear = true);
    /// This function deletes all the Texture present int the map
    static void ResetTextures();
    
    // -----------
    // Variables
    // -----------
    static std::unordered_map<std::string, std::shared_ptr<Texture>> texture_library_;
    
    MAKE_PURE_STATIC(TextureLibrary)
    
    friend class Renderer;
  };
  
}
