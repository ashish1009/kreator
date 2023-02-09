//
//  texture.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

#include "renderer/utils/renderer.hpp"
#include "editor/property_grid.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ikan {
  
  enum class TextureFormat  {
    None = 0, RGBA
  };
  
  class Texture;
  class SubTexture;
  
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
    ///   - linear: min linear flag
    [[nodiscard]] static std::shared_ptr<Texture> Create(const std::string& file_path, bool linear = true);
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
    ///   - linear: min linear flag
    [[nodiscard]] static std::shared_ptr<Texture> GetTexture(const std::string& path, bool linear = true);
    /// This function deletes all the Texture present int the map
    static void ResetTextures();
    
    // -----------
    // Variables
    // -----------
    // Array of 2 to keep both linear and nearest min and mag flags
    // 0 -> Linear Filter
    // 1 -> Nearest Filter
    static std::unordered_map<std::string, std::array<std::shared_ptr<Texture>, 2>> texture_library_;
    
    MAKE_PURE_STATIC(TextureLibrary)
    
    friend class Renderer;
  };
  
  /// This structure holds the texture component with use flag
  struct TextureComponent {
    bool use = false;
    bool use_sprite = false;
    bool linear_edge = true;
    
    std::shared_ptr<Texture> component = nullptr;
    std::shared_ptr<SubTexture> sprite = nullptr;
    
    float tiling_factor = 1.0f;
    
    // default constructor and destructors
    TextureComponent() = default;
    ~TextureComponent() noexcept = default;
    
    // Copy and Move Constructor and operator =
    DEFINE_COPY_MOVE_CONSTRUCTORS(TextureComponent)
    
    // Parameter constructor
    /// This constructure creates the texture component
    /// - Parameter comp: component of texture
    TextureComponent(const std::shared_ptr<Texture>& comp, bool use = true);
    /// This function loads the textrue and sprite again
    /// - Parameter other: component
    void LoadTexture(const TextureComponent& other);
    /// This function changes the linear flag of texture
    void ChangeLinearTexture();
    
    template<typename UIFunction>
    /// This function renders the texture components
    /// - Parameters:
    ///   - color: color of the texture
    ///   - ui_function: function to render below texture Use
    void RenderGui(glm::vec4& color, UIFunction ui_function) {
      ImGui::PushID("##PropertyGrid::TextureComponent");
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, 60);
      
      static std::shared_ptr<Texture> no_texture = Renderer::GetTexture(AM::CoreAsset("textures/default/no_texture.png"));
      size_t tex_id = ((component) ? component->GetRendererID() : no_texture->GetRendererID());
      
      // Show the image of texture
      ImGui::Image((void*)tex_id, ImVec2(40.0f, 40.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f),
                   ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
      
      // Drop the texture here and load new texture
      PropertyGrid::DropConent([this](const std::string& path)
                               {
        component.reset();
        sprite.reset();
        
        LoadTexture(Renderer::GetTexture(path));
      });
      // TODO: Add hovered larger Image
      PropertyGrid::HoveredMsg("Drop the Texture file in the Image Button to "
                               "upload the texture");
      ImGui::NextColumn();
      
      // Check box to togle use of texture
      ImGui::Checkbox("Use ", &use);
      if (use) {
        ImGui::SameLine();
        // Check box to togle use of texture
        if (ImGui::Checkbox("Linear Edge", &linear_edge)) {
          ChangeLinearTexture();
        }
        PropertyGrid::HoveredMsg("Enable to Render the Sprite out the Texture");
        
        ImGui::SameLine();
        // Check box to togle use of texture
        ImGui::Checkbox("Sprite", &use_sprite);
        PropertyGrid::HoveredMsg("Enable to Render the Sprite out the Texture");
      }
      
      ui_function();
      
      if (use and !use_sprite) {
        ImGui::SameLine();
        ImGui::DragFloat("", &tiling_factor, 1.0f, 1.0f, 1000.0f);
        PropertyGrid::HoveredMsg("Tiling Factor");
      }
      
      ImGui::Columns(1);
      ImGui::Separator();
      ImGui::PopID();
    }

  };
  
}
