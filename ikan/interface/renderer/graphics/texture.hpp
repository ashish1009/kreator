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
    virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    virtual uint32_t GetHeight() const = 0;
    /// This function returns the File Path of Texture NOTE: Return "" for white texture
    virtual const std::string& GetfilePath() const = 0;
    /// This function returns name of texture
    virtual const std::string& GetName() const = 0;

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
    virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    virtual uint32_t GetHeight() const = 0;
    /// This function returns the Size of Freetpe face
    virtual glm::ivec2 GetSize() const = 0;
    /// This function returns the Bearing of Freetpe face
    virtual glm::ivec2 GetBearing() const = 0;
    /// This function returns the Advance of Freetpe face
    virtual uint32_t GetAdvance() const = 0;
    
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
    static std::shared_ptr<CharTexture> Create(const FT_Face& face,
                                               const glm::ivec2& size,
                                               const glm::ivec2& bearing,
                                               uint32_t advance,
                                               [[maybe_unused]] char char_val);
  };
  
  /// This class stores the compiled shader in library
  class TextureLibrary {
  private:
    // -----------
    // Functions
    // -----------
    /// This function returns the Ref type of ikan::Shader. It creates a new if not present in the map
    /// - Parameters:
    ///   - path: path of textre
    ///   - min_linear: min linear flag
    ///   - mag_linear: max linear flag
    static std::shared_ptr<Texture> GetShader(const std::string& path,
                                              bool min_linear = true,
                                              bool mag_linear = true);
    /// This function deletes all the shaders present int the map
    static void ResetShaders();
    
    // -----------
    // Variables
    // -----------
    static std::unordered_map<std::string, std::shared_ptr<Texture>> shader_library_;
    
    MAKE_PURE_STATIC(TextureLibrary)
    
    friend class Renderer;
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
    
    // -------------
    // Getters
    // -------------
    /// This function returns the Renderer ID of Texture
    virtual RendererID GetRendererID() const = 0;
    /// This function returns the Width of Texture
    virtual uint32_t GetWidth() const = 0;
    /// This function returns the Height of Texture
    virtual uint32_t GetHeight() const = 0;
    /// This function returns the Size of Freetpe face
    virtual uint32_t GetSize() const = 0;
    /// This function loads the data in GPU
    /// - Parameters:
    ///   - width: width of image
    ///   - height: height of image
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    /// This static functions creates the Texture without data of size
    /// - Parameters:
    ///   - width: width of image
    ///   - height: height of image
    ///   - format: image format
    ///   - data: data of image
    static std::shared_ptr<Image> Create(uint32_t width,
                                         uint32_t height,
                                         TextureFormat format,
                                         void* data = nullptr);
  };
  
}
