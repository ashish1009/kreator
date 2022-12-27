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
    static constexpr uint32_t VertexForSingleChar = 6;
    
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
    glm::vec2 base_texture_coords[VertexForSingleChar];
    
    /// Constructor
    TextData() {
      IK_CORE_TRACE("Creating Text Data instance ...");
    }
    /// Destructir
    virtual ~TextData() {
      IK_CORE_WARN("Destroying Text Data instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= TextData::VertexForSingleChar * sizeof(TextData::Vertex);
    }
  };
  static TextData* text_data_;
  
  void TextRenderer::Init() {
    text_data_ = new TextData();
    
    // Allocating the memory for vertex Buffer Pointer
    text_data_->vertex_buffer_base_ptr = new TextData::Vertex[TextData::VertexForSingleChar];
    
    // Create Pipeline instance
    text_data_->pipeline = Pipeline::Create();
    
    // Create vertes Buffer
    text_data_->vertex_buffer = VertexBuffer::Create(sizeof(TextData::Vertex) * TextData::VertexForSingleChar);
    text_data_->vertex_buffer->AddLayout({
      { "a_Position",  ShaderDataType::Float3 },
      { "a_Color",     ShaderDataType::Float4 },
      { "a_TexCoords", ShaderDataType::Float2 },
      { "a_ObjectID",  ShaderDataType::Int },
    });
    text_data_->pipeline->AddVertexBuffer(text_data_->vertex_buffer);
    
    // Settingup shader
    text_data_->shader = Renderer::GetShader(AssetManager::CoreAsset("shaders/text_shader.glsl"));
    
    // Base Texture coordinate for Char rendering
    text_data_->base_texture_coords[0] = { 0.0f, 0.0f };
    text_data_->base_texture_coords[1] = { 0.0f, 1.0f };
    text_data_->base_texture_coords[2] = { 1.0f, 1.0f };
    text_data_->base_texture_coords[3] = { 0.0f, 0.0f };
    text_data_->base_texture_coords[4] = { 1.0f, 1.0f };
    text_data_->base_texture_coords[5] = { 1.0f, 0.0f };
    
    IK_CORE_INFO("Initialised the Text Renderer");
    IK_CORE_INFO("  ---------------------------------------------------------");
    IK_CORE_INFO("  Vertex Buffer Used | {0} B ({1} KB) ",
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex),
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex) / 1000.0f );
    IK_CORE_INFO("  Shader used        | {0}", text_data_->shader->GetName());
    IK_CORE_INFO("  ---------------------------------------------------------");
  }
  
  void TextRenderer::Shutdown() {
    IK_CORE_WARN("Shutting down the Text Renderer !!!");
    IK_CORE_WARN("  ---------------------------------------------------------");
    IK_CORE_WARN("  Vertex Buffer Used | {0} B ({1} KB) ",
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex),
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex) / 1000.0f );
    IK_CORE_WARN("  Shader used        | {0}", text_data_->shader->GetName());
    IK_CORE_WARN("  ---------------------------------------------------------");

    delete text_data_;
  }
  
}
