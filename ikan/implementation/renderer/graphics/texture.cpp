//
//  texture.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "texture.hpp"
#include "platform/open_gl/open_gl_texture.hpp"
#include "editor/property_grid.hpp"

namespace ikan {
  
  // --------------------------------------------------------------------------
  // Texture Create API
  // --------------------------------------------------------------------------
  std::shared_ptr<Texture> Texture::Create(const std::string& file_path, bool linear) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLTexture>(file_path, linear);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  std::shared_ptr<Texture> Texture::Create(uint32_t width,
                                           uint32_t height,
                                           void* data,
                                           uint32_t size) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLTexture>(width, height, data, size);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Character texture
  // --------------------------------------------------------------------------
  std::shared_ptr<CharTexture> CharTexture::Create(const FT_Face& face,
                                                   const glm::ivec2& size,
                                                   const glm::ivec2& bearing,
                                                   uint32_t advance,
                                                   [[maybe_unused]] char char_val) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLCharTexture>(face, size, bearing, advance, char_val);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Image API
  // --------------------------------------------------------------------------
  std::shared_ptr<Image> Image::Create(uint32_t width,
                                       uint32_t height,
                                       TextureFormat format,
                                       void* data) {
    switch (Renderer::GetApi()) {
      case Renderer::Api::OpenGl:
        return std::make_shared<OpenGLImage>(width, height, format, data);
      case Renderer::Api::None:
      default:
        IK_CORE_ASSERT(false, "Invalid Renderer API (None)"); break;
    }
  }
  
  // --------------------------------------------------------------------------
  // Texture Component
  // --------------------------------------------------------------------------
  void TextureComponent::LoadTexture(const TextureComponent& other) {
    if (other.component) {
      component = Renderer::GetTexture(other.component->GetfilePath(), other.linear_edge);
      sprite = SubTexture::CreateFromCoords(component, other.sprite->GetCoords(), other.sprite->GetSpriteSize(), other.sprite->GetCellSize());
    }
    else {
      component = nullptr;
      sprite = nullptr;
    }
  }
  void TextureComponent::ChangeLinearTexture() {
    // Need to copy so not using reference as texture will deleted
    const std::string tex_path = component->GetfilePath();
    const glm::vec2 coords = sprite->GetCoords();
    const glm::vec2 sprite_size = sprite->GetSpriteSize();
    const glm::vec2 cell_size = sprite->GetCellSize();
    
    component.reset();
    sprite.reset();
    
    component = Renderer::GetTexture(tex_path, linear_edge);
    sprite = SubTexture::CreateFromCoords(component, coords, sprite_size, cell_size);
  }
  
  TextureComponent::TextureComponent(const std::shared_ptr<Texture>& comp, bool use)
  : component(comp), use(use) {
    IK_CORE_TRACE(LogModule::Texture, "Copying TextureComponent");
    if (component) {
      sprite = SubTexture::CreateFromCoords(component, {0.0f, 0.0f});
    }
  }
  
  TextureComponent::TextureComponent(const TextureComponent& other)
  : use(other.use), linear_edge(other.linear_edge), use_sprite(other.use_sprite), tiling_factor(other.tiling_factor) {
    IK_CORE_TRACE(LogModule::Texture, "Copying TextureComponent");
    LoadTexture(other);
  }
  
  TextureComponent::TextureComponent(TextureComponent&& other)
  : use(other.use), linear_edge(other.linear_edge), use_sprite(other.use_sprite), tiling_factor(other.tiling_factor) {
    IK_CORE_TRACE(LogModule::Texture, "Moving TextureComponent");
    LoadTexture(other);
  }
  
  TextureComponent& TextureComponent::operator=(const TextureComponent& other) {
    use = other.use;
    linear_edge = other.linear_edge;
    use_sprite = other.use_sprite;
    tiling_factor = other.tiling_factor;
    
    LoadTexture(other);

    IK_CORE_TRACE(LogModule::Texture, "Copying TextureComponent (=operator)");
    return *this;
  }
  
  TextureComponent& TextureComponent::operator=(TextureComponent&& other) {
    use = other.use;
    linear_edge = other.linear_edge;
    use_sprite = other.use_sprite;
    tiling_factor = other.tiling_factor;
    
    LoadTexture(other);

    IK_CORE_TRACE(LogModule::Texture, "Moving TextureComponent (=operator)");
    return *this;
  }

  // --------------------------------------------------------------------------
  // Texture Library
  // --------------------------------------------------------------------------
  std::unordered_map<std::string, std::array<std::shared_ptr<Texture>, 2>> TextureLibrary::texture_library_;
  
  std::shared_ptr<Texture> TextureLibrary::GetTexture(const std::string& path, bool linear) {
    if (texture_library_.find(path) == texture_library_.end() or !texture_library_[path][linear]) {
      texture_library_[path][linear] = Texture::Create(path, linear);
      IK_CORE_DEBUG(LogModule::Texture, "Adding Texture '{0}' to Shdaer Library", StringUtils::GetNameFromFilePath(path));
      IK_CORE_DEBUG(LogModule::Texture, "Number of Textures loaded yet {0}", texture_library_.size());
    } else {
      IK_CORE_DEBUG(LogModule::Texture, "Returning Pre loaded Texture '{0}' from Shdaer Library", StringUtils::GetNameFromFilePath(path));
    }
    
    return texture_library_.at(path)[linear];
  }
  
  void TextureLibrary::ResetTextures() {
    for (auto it = texture_library_.begin(); it != texture_library_.end(); it++) {
      IK_CORE_TRACE(LogModule::Texture, "Removing Texture '{0}' from Shdaer Library",
                   StringUtils::GetNameFromFilePath(it->first));
      for (int i = 0; i < 2; i++)
        it->second[i].reset();
    }
  }

  // --------------------------------------------------------------------------
  // Sub Texture API
  // --------------------------------------------------------------------------
  SubTexture::SubTexture(const std::shared_ptr<Texture>& sprite_image,
                         const glm::vec2& min,
                         const glm::vec2& max,
                         const glm::vec2& coords,
                         const glm::vec2& sprite_size,
                         const glm::vec2& cell_size)
  : sprite_image_(sprite_image), sprite_size_(sprite_size), cell_size_(cell_size), coords_(coords) {
    texture_coord_[0] = {min.x, min.y};
    texture_coord_[1] = {max.x, min.y};
    texture_coord_[2] = {max.x, max.y};
    texture_coord_[3] = {min.x, max.y};
  }
  
  std::shared_ptr<SubTexture> SubTexture::CreateFromCoords(const std::shared_ptr<Texture>& sprite_image,
                                                           const glm::vec2& coords,
                                                           const glm::vec2& sprite_size,
                                                           const glm::vec2& cell_size) {
    glm::vec2 min = {
      (coords.x * cell_size.x) / sprite_image->GetWidth(),
      (coords.y * cell_size.y) / sprite_image->GetHeight()
    };
    glm::vec2 max = {
      ((coords.x + sprite_size.x) * cell_size.x) / sprite_image->GetWidth(),
      ((coords.y + sprite_size.y) * cell_size.y) / sprite_image->GetHeight()
    };
    
    IK_CORE_TRACE(LogModule::SubTexture, "Creating Sub Texture with following Data ");
    IK_CORE_TRACE(LogModule::SubTexture, "  Sprite Image | {0}", sprite_image->GetName());
    IK_CORE_TRACE(LogModule::SubTexture, "  Coordinates  | {0} : {1}", coords.x, coords.y);
    IK_CORE_TRACE(LogModule::SubTexture, "  Sprite Size  | {0} : {1}", sprite_size.x, sprite_size.y);
    IK_CORE_TRACE(LogModule::SubTexture, "  Cell Size    | {0} : {1}", cell_size.x, cell_size.y);
    IK_CORE_TRACE(LogModule::SubTexture, "  Min Bound    | {0} : {1}", min.x, min.y);
    IK_CORE_TRACE(LogModule::SubTexture, "  Max Bound    | {0} : {1}", max.x, max.y);
    
    return std::make_shared<SubTexture>(sprite_image, min, max, coords, sprite_size, cell_size);
  }

  std::shared_ptr<Texture> SubTexture::GetSpriteImage() { return sprite_image_; }
  const glm::vec2* SubTexture::GetTexCoord() const { return texture_coord_; }
  glm::vec2& SubTexture::GetSpriteSize() { return sprite_size_; }
  glm::vec2& SubTexture::GetCellSize() { return cell_size_; }
  glm::vec2& SubTexture::GetCoords() { return coords_; }

}
