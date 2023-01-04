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
      IK_CORE_TRACE("Text", "Creating Text Data instance ...");
    }
    /// Destructir
    virtual ~TextData() {
      IK_CORE_WARN("Text", "Destroying Text Data instance and clearing the data !!!");
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
    
    IK_CORE_INFO("Text", "Initialised the Text Renderer");
    IK_CORE_INFO("Text", "  ---------------------------------------------------------");
    IK_CORE_INFO("Text", "  Vertex Buffer Used | {0} B ({1} KB) ",
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex),
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex) / 1000.0f );
    IK_CORE_INFO("Text", "  Shader used        | {0}", text_data_->shader->GetName());
    IK_CORE_INFO("Text", "  ---------------------------------------------------------");
  }
  
  void TextRenderer::Shutdown() {
    IK_CORE_WARN("Text", "Shutting down the Text Renderer !!!");
    IK_CORE_WARN("Text", "  ---------------------------------------------------------");
    IK_CORE_WARN("Text", "  Vertex Buffer Used | {0} B ({1} KB) ",
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex),
                 TextData::VertexForSingleChar * sizeof(TextData::Vertex) / 1000.0f );
    IK_CORE_WARN("Text", "  Shader used        | {0}", text_data_->shader->GetName());
    IK_CORE_WARN("Text", "  ---------------------------------------------------------");

    delete text_data_;
  }
  
  void TextRenderer::LoadFreetype(const std::string& font_file_path) {
    IK_CORE_DEBUG("Loading the Font for Text renderer | '{0}'", font_file_path.c_str());
    
    FT_Library ft;
    IK_CORE_ASSERT(!FT_Init_FreeType(&ft), "Not able to load Freetype");
    
    // find path to font
    IK_CORE_ASSERT(!font_file_path.empty(), "Not able to load Font");
    
    // load font as face
    FT_Face face;
    IK_CORE_ASSERT(!FT_New_Face(ft, font_file_path.c_str(), 0, &face),
                   "Unavle to load tha font to freetype");
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) {
      // Load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
        IK_CORE_ERROR("Text", "ERROR::FREETYTPE: Failed to load Glyph");
        continue;
      }
      // now store character for later use
      std::shared_ptr<CharTexture> char_texture = CharTexture::Create(face,
                                                                      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                                                                      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                                                                      static_cast<uint32_t>(face->glyph->advance.x),
                                                                      char(c));
      text_data_->char_texture_map.insert(std::pair<char, std::shared_ptr<CharTexture>>(c, char_texture));
    }
    
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
  }
  
  void TextRenderer::RenderText(std::string text,
                                const glm::mat4& view_projection_camera,
                                glm::vec3 position,
                                const glm::vec2& scale_,
                                const glm::vec4& color) {
    // Update camera to shader
    text_data_->shader->Bind();
    text_data_->shader->SetUniformMat4("u_ViewProjection", view_projection_camera);
    
    // iterate through all characters
    std::string::const_iterator c;
    
    // Rescaling the text as it render too large in begining
    glm::vec2 scale = scale_ * glm::vec2(1.00f);
    
    for (c = text.begin(); c != text.end(); c++) {
      std::shared_ptr<CharTexture> ch = text_data_->char_texture_map[*c];
      
      float xpos = position.x + ch->GetBearing().x * scale.x;
      float ypos = position.y - (ch->GetSize().y - ch->GetBearing().y) * scale.y;
      float zpos = position.z;
      
      float w = ch->GetSize().x * scale.x;
      float h = ch->GetSize().y * scale.y;
      
      // update VBO for each character
      glm::vec3 vertex_position[TextData::VertexForSingleChar] = {
        { xpos,     ypos + h, zpos },
        { xpos,     ypos    , zpos },
        { xpos + w, ypos    , zpos },
        
        { xpos,     ypos + h, zpos },
        { xpos + w, ypos    , zpos },
        { xpos + w, ypos + h, zpos },
      };
      
      text_data_->vertex_buffer_ptr = text_data_->vertex_buffer_base_ptr;
      
      // Each Vertex of Char
      for (size_t i = 0; i < TextData::VertexForSingleChar; i++) {
        text_data_->vertex_buffer_ptr->position      = vertex_position[i];
        text_data_->vertex_buffer_ptr->color         = color;
        text_data_->vertex_buffer_ptr->texture_coord = text_data_->base_texture_coords[i];
        text_data_->vertex_buffer_ptr->object_id     = -1;
        text_data_->vertex_buffer_ptr++;
      }
      
      uint32_t dataSize = (uint32_t)((uint8_t*)text_data_->vertex_buffer_ptr - (uint8_t*)text_data_->vertex_buffer_base_ptr);
      text_data_->vertex_buffer->SetData(text_data_->vertex_buffer_base_ptr, dataSize);
      
      // Render the Scene
      text_data_->shader->Bind();
      ch->Bind();
      Renderer::DrawArrays(text_data_->pipeline, 6);
      
      // now advance cursors for next glyph (note that advance is number of
      // 1/64 pixels) bitshift by 6 to get value in pixels (2^6 = 64 (divide
      // amount of 1/64th pixels by 64 to get amount of pixels))
      position.x += (ch->GetAdvance() >> 6) * scale.x;
      
      // Renderer Vertex count stat
      RendererStatistics::Get().vertex_count += TextData::VertexForSingleChar;
    }
  }
}
