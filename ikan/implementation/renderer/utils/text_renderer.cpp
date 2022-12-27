//
//  text_renderer.cpp
//  ikan
//
//  Created by Ashish . on 28/12/22.
//

#include "text_renderer.hpp"
#include "renderer/graphics/shader.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/graphics/texture.hpp"
#include "renderer/utils/renderer.hpp"
#include "renderer/utils/renderer_stats.hpp"

namespace ikan {
  
  struct TextData {
    /// Vertex Information for rendering Text
    struct Vertex {
      glm::vec3 position;
      glm::vec4 color;
      glm::vec2 texture_coord;
      int32_t object_id;
    };
    
    // Fixed Constants
    static constexpr uint32_t kVertexForSingleChar = 6;
    
    /// Renderer Data storage
    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::shared_ptr<Shader> shader;
    
    /// Map to store the Char Texture for each character
    std::map<char, std::shared_ptr<CharTexture>> char_texture_map;
    
    // -------------- Variables ------------------
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Store the Base Texture coordinga
    glm::vec2 base_texture_coords[kVertexForSingleChar];
    
    /// Constructor
    TextData() {
      IK_CORE_TRACE("Creating Text Data instance ...");
    }
    /// Destructir
    virtual ~TextData() {
      IK_CORE_WARN("Destroying Text Data instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= TextData::kVertexForSingleChar * sizeof(TextData::Vertex);
    }
  };
  static TextData* text_data_;
  
  void TextRenderer::Init() {
  }
  
  void TextRenderer::Shutdown() {
  }
  
}
