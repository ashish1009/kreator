//
//  batch_2d_renderer.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "batch_2d_renderer.hpp"
#include "renderer/utils/renderer_stats.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/graphics/renderer_buffer.hpp"
#include "renderer/graphics/shader.hpp"
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  static constexpr glm::vec2 texture_coords_[] = {
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
  };
  
  struct BaseVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texture_coords;
    
    float texture_index;
    float tiling_factor;
  };
  
  /// This structure holds the common batch renderer data for Quads, circle and lines
  struct CommonData {
    /// Renderer Data storage
    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::shared_ptr<Shader> shader;
    
    /// Max element to be rendered in single batch
    uint32_t max_element = 0;
    uint32_t max_vertices = 0;
    
    friend struct BatchRendererData;
    friend struct LineData;
    
  private:
    CommonData() = default;
  };
  
  /// This structure holds the common batch renderer data for Quads and circle
  struct BatchRendererData : CommonData{
    // Constants
    static constexpr uint32_t VertexForSingleElement = 4;
    static constexpr uint32_t IndicesForSingleElement = 6;
    
    /// This structure stores the environmant data
    struct Environment {
      glm::mat4 camera_view_projection_matrix;
    };
    
    /// Store the Vertex and Indices size
    uint32_t max_indices = 0;
    
    /// Store the Environment data
    Environment environment;
    
    /// Count of Indices to be renderer in Single Batch
    uint32_t index_count = 0;
    
    /// Stores all the 16 Texture in array so that there is no need to load texture each frame
    /// NOTE: Load only if new texture is added or older replaced with new
    std::array<std::shared_ptr<Texture>, kMaxTextureSlotsInShader> texture_slots;
    
    /// Texture Slot index sent to Shader to render a specific Texture from slots
    /// Slot 0 is reserved for white texture (No Image only color)
    uint32_t texture_slot_index = 1; // 0 = white texture
    
    /// Basic vertex of quad
    /// Vertex of circle is taken as Quad only
    glm::vec4 vertex_base_position[4];
    
    void StartCommonBatch() {
      index_count = 0;
      texture_slot_index = 1;
    }
    
    /// Virtual Destructor
    virtual ~BatchRendererData() {
      RendererStatistics::Get().index_buffer_size -= max_indices * sizeof(uint32_t);
    }
    
    friend struct QuadData;
    friend struct CircleData;
    
  private:
    BatchRendererData() = default;
  };
  
  /// Batch Data to Rendering Quads
  struct QuadData : BatchRendererData {
    /// Single vertex of a Quad
    struct Vertex : BaseVertex {
      int32_t object_id;        // Pixel ID of Quad
    };
    
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Constructor
    QuadData() {
      IK_CORE_TRACE(LogModule::Batch2DRenderer, "Creating QuadData instance ...");
    }
    /// Destructor
    virtual ~QuadData() {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying QuadData instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= max_vertices * sizeof(QuadData::Vertex);
    }
    
    /// start new batch for quad rendering
    void StartBatch() {
      StartCommonBatch();
      vertex_buffer_ptr = vertex_buffer_base_ptr;
    }
  };
  static QuadData* quad_data_;
  
  /// Batch Data to Rendering Circles
  struct CircleData : BatchRendererData {
    /// Single vertex of a Circle
    struct Vertex : BaseVertex {
      glm::vec3 local_position;
      float thickness;    // Thickness of Circle
      float fade;         // Fadeness of Edge of Circle
      
      int32_t object_id; // Pixel ID of Quad
    };
    
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Constructor
    CircleData() {
      IK_CORE_TRACE(LogModule::Batch2DRenderer, "Creating CircleData instance ...");
    }
    /// Destructir
    virtual ~CircleData() {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying Circle Data instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= CircleData::max_vertices * sizeof(CircleData::Vertex);
    }
    
    /// start new batch for quad rendering
    void StartBatch() {
      StartCommonBatch();
      vertex_buffer_ptr = vertex_buffer_base_ptr;
    }
  };
  static CircleData* circle_data_;
  
  struct LineData : CommonData {
    /// Single vertex of a Circle
    struct Vertex {
      glm::vec3 position;       // Position of a Quad
      glm::vec4 color;          // Color of a Quad
    };
    
    static constexpr uint32_t kVertexForSingleLine = 2;
    
    /// Count of Indices to be renderer in Single Batch
    uint32_t vertex_count = 0;
    
    /// Base pointer of Vertex Data. This is start of Batch data for single draw call
    Vertex* vertex_buffer_base_ptr = nullptr;
    /// Incrememntal Vetrtex Data Pointer to store all the batch data in Buffer
    Vertex* vertex_buffer_ptr = nullptr;
    
    /// Constructor
    LineData() {
      IK_CORE_TRACE(LogModule::Batch2DRenderer, "Creating Line Data instance ...");
    }
    /// Destructir
    virtual ~LineData() {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying Line Data instance and clearing the data !!!");
      delete [] vertex_buffer_base_ptr;
      vertex_buffer_base_ptr = nullptr;
      
      RendererStatistics::Get().vertex_buffer_size -= LineData::max_vertices * sizeof(LineData::Vertex);
    }
    
    /// start new batch for quad rendering
    void StartBatch() {
      vertex_count = 0;
      vertex_buffer_ptr = vertex_buffer_base_ptr;
    }
  };
  static LineData* line_data_;
  
  // --------------------------------------------------------------------------
  // Batch Renderer API
  // --------------------------------------------------------------------------
  void BatchRenderer::Init(uint32_t max_quads, uint32_t max_cirlces, uint32_t max_lines) {
    IK_CORE_INFO(LogModule::Batch2DRenderer, "Initialising the Batch Renderer 2D ...");
    InitQuadData(max_quads);
    InitCircleData(max_cirlces);
    InitLineData(max_lines);
  }
  
  void BatchRenderer::Shutdown() {
    IK_CORE_WARN(LogModule::Batch2DRenderer, "Shutting Down the Batch Renderer 2D !!!");
    
    if (quad_data_) {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying the Batch Renderer Quad Data");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Quads per Batch             | {0}", quad_data_->max_element);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                   quad_data_->max_vertices * sizeof(QuadData::Vertex), quad_data_->max_vertices * sizeof(QuadData::Vertex) / 1000.0f );
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Index Buffer used               | {0} B ({1} KB) ",
                   quad_data_->max_indices * sizeof(uint32_t), quad_data_->max_indices * sizeof(uint32_t) / 1000.0f );
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Shader Used                     | {0}", quad_data_->shader->GetName());
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      delete quad_data_;
    }
    
    if (circle_data_) {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying the Batch Renderer Circle Data");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Circles per Batch           | {0}", circle_data_->max_element);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Texture Slots Batch         | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                   circle_data_->max_vertices * sizeof(CircleData::Vertex),
                   circle_data_->max_vertices * sizeof(CircleData::Vertex) / 1000.0f );
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                   circle_data_->max_indices * sizeof(uint32_t),
                   circle_data_->max_indices * sizeof(uint32_t) / 1000.0f );
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Shader used                     | {0}", circle_data_->shader->GetName());
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      delete circle_data_;
    }
    if (line_data_) {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Destroying the Batch Renderer Line Data");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max Lines per Batch             | {0}", line_data_->max_element);
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                   line_data_->max_vertices * sizeof(LineData::Vertex),
                   line_data_->max_vertices * sizeof(LineData::Vertex) / 1000.0f );
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  Shader used                   x  | {0}", line_data_->shader->GetName());
      IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
      delete line_data_;
    }
    
  }
  
  void BatchRenderer::InitQuadData(uint32_t max_quads) {
    uint32_t prev_max_quads = 0;
    if (quad_data_)
      prev_max_quads = quad_data_->max_element;
    
    if (max_quads == 0)
      return;
    
    max_quads += prev_max_quads;
    
    delete quad_data_;
    quad_data_ = nullptr;
    
    // Allocate memory for Quad Data
    quad_data_ = new QuadData();

    quad_data_->max_element = max_quads;
    quad_data_->max_vertices = max_quads * BatchRendererData::VertexForSingleElement;
    quad_data_->max_indices = max_quads * BatchRendererData::IndicesForSingleElement;
    
    // Create Pipeline instance
    quad_data_->pipeline = Pipeline::Create();
    
    // Allocating the memory for vertex Buffer Pointer
    quad_data_->vertex_buffer_base_ptr = new QuadData::Vertex[quad_data_->max_vertices];
    
    // Create vertes Buffer
    quad_data_->vertex_buffer = VertexBuffer::Create(quad_data_->max_vertices * sizeof(QuadData::Vertex));
    quad_data_->vertex_buffer->AddLayout({
      { "a_Position",     ShaderDataType::Float3 },
      { "a_Color",        ShaderDataType::Float4 },
      { "a_TexCoords",    ShaderDataType::Float2 },
      { "a_TexIndex",     ShaderDataType::Float },
      { "a_TilingFactor", ShaderDataType::Float },
      { "a_ObjectID",     ShaderDataType::Int },
    });
    quad_data_->pipeline->AddVertexBuffer(quad_data_->vertex_buffer);
    
    // Setup the Quad Shader
    quad_data_->shader = Renderer::GetShader(AM::CoreAsset("shaders/batch_quad_shader.glsl"));
    
    // Create Index Buffer
    uint32_t* quad_indices = new uint32_t[quad_data_->max_indices];
    
    uint32_t offset = 0;
    for (size_t i = 0; i < quad_data_->max_indices; i += BatchRendererData::IndicesForSingleElement) {
      quad_indices[i + 0] = offset + 0;
      quad_indices[i + 1] = offset + 1;
      quad_indices[i + 2] = offset + 2;
      
      quad_indices[i + 3] = offset + 2;
      quad_indices[i + 4] = offset + 3;
      quad_indices[i + 5] = offset + 0;
      
      offset += 4;
    }
    
    // Create Index Buffer in GPU for storing Indices
    std::shared_ptr<IndexBuffer> ib = IndexBuffer::CreateWithCount(quad_indices, quad_data_->max_indices);
    quad_data_->pipeline->SetIndexBuffer(ib);
    delete[] quad_indices;
    
    // Creating white texture for colorful quads witout any texture or sprite
    uint32_t whiteTextureData = 0xffffffff;
    quad_data_->texture_slots[0] = Texture::Create(1, 1, &whiteTextureData, sizeof(uint32_t));
    
    // Setting basic Vertex point of quad
    quad_data_->vertex_base_position[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    quad_data_->vertex_base_position[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    quad_data_->vertex_base_position[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    quad_data_->vertex_base_position[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    
    Renderer2DStats::Get().max_quads = quad_data_->max_element;

    IK_CORE_INFO(LogModule::Batch2DRenderer, "Initialized Batch Renderer for Quad Data");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max Quads per Batch             | {0}", quad_data_->max_element);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                 quad_data_->max_vertices * sizeof(QuadData::Vertex), quad_data_->max_vertices * sizeof(QuadData::Vertex) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Index Buffer used               | {0} B ({1} KB) ",
                 quad_data_->max_indices * sizeof(uint32_t), quad_data_->max_indices * sizeof(uint32_t) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Shader Used                     | {0}", quad_data_->shader->GetName());
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
  }
  
  void BatchRenderer::InitCircleData(uint32_t max_circles) {
    uint32_t prev_max_circle = 0;
    if (circle_data_)
      prev_max_circle = circle_data_->max_element;
    
    if (max_circles == 0)
      return;
    
    max_circles += prev_max_circle;

    delete circle_data_;
    circle_data_ = nullptr;
    
    // Allocate memory for Circle Data
    circle_data_ = new CircleData();
        
    circle_data_->max_element = max_circles;
    circle_data_->max_vertices = max_circles * BatchRendererData::VertexForSingleElement;
    circle_data_->max_indices = max_circles * BatchRendererData::IndicesForSingleElement;
    
    // Allocating the memory for vertex Buffer Pointer
    circle_data_->vertex_buffer_base_ptr = new CircleData::Vertex[circle_data_->max_vertices];
    
    // Create Pipeline instance
    circle_data_->pipeline = Pipeline::Create();
    
    // Create vertes Buffer
    circle_data_->vertex_buffer = VertexBuffer::Create(circle_data_->max_vertices * sizeof(CircleData::Vertex));
    circle_data_->vertex_buffer->AddLayout({
      { "a_Position",     ShaderDataType::Float3 },
      { "a_Color",        ShaderDataType::Float4 },
      { "a_TexCoords",    ShaderDataType::Float2 },
      { "a_TexIndex",     ShaderDataType::Float },
      { "a_TilingFactor", ShaderDataType::Float },
      { "a_LocalPosition",ShaderDataType::Float3 },
      { "a_Thickness",    ShaderDataType::Float },
      { "a_Fade",         ShaderDataType::Float },
      { "a_ObjectID",     ShaderDataType::Int },
    });
    circle_data_->pipeline->AddVertexBuffer(circle_data_->vertex_buffer);
    
    // Create Index Buffer
    uint32_t* quad_indices = new uint32_t[circle_data_->max_indices];
    
    uint32_t offset = 0;
    for (size_t i = 0; i < circle_data_->max_indices; i += BatchRendererData::IndicesForSingleElement) {
      quad_indices[i + 0] = offset + 0;
      quad_indices[i + 1] = offset + 1;
      quad_indices[i + 2] = offset + 2;
      
      quad_indices[i + 3] = offset + 2;
      quad_indices[i + 4] = offset + 3;
      quad_indices[i + 5] = offset + 0;
      
      offset += 4;
    }
    
    // Create Index Buffer in GPU for storing Indices
    std::shared_ptr<IndexBuffer> ib = IndexBuffer::CreateWithCount(quad_indices, circle_data_->max_indices);
    circle_data_->pipeline->SetIndexBuffer(ib);
    delete[] quad_indices;
    
    // Creating white texture for colorful quads witout any texture or sprite
    uint32_t whiteTextureData = 0xffffffff;
    circle_data_->texture_slots[0] = Texture::Create(1, 1, &whiteTextureData,
                                                     sizeof(uint32_t));
    
    // Setting basic Vertex point of quad
    circle_data_->vertex_base_position[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    circle_data_->vertex_base_position[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    circle_data_->vertex_base_position[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    circle_data_->vertex_base_position[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    
    // Setup the Circle Shader
    circle_data_->shader = Renderer::GetShader(AM::CoreAsset("shaders/batch_circle_shader.glsl"));
    
    Renderer2DStats::Get().max_circles = circle_data_->max_element;
    
    IK_CORE_INFO(LogModule::Batch2DRenderer, "Initialized Batch Renderer for Circle Data");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max Circle per Batch            | {0}", max_circles);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                 circle_data_->max_vertices * sizeof(CircleData::Vertex), circle_data_->max_vertices * sizeof(CircleData::Vertex) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                 circle_data_->max_indices * sizeof(uint32_t), circle_data_->max_indices * sizeof(uint32_t) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Shader used                     | {0}", circle_data_->shader->GetName());
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
  }
  
  void BatchRenderer::InitLineData(uint32_t max_lines) {
    uint32_t prev_max_lines = 0;
    if (line_data_)
      prev_max_lines = line_data_->max_element;
    
    if (max_lines == 0)
      return;
    
    max_lines += prev_max_lines;
    
    delete line_data_;
    line_data_ = nullptr;
    
    // Allocate memory for Quad Data
    line_data_ = new LineData();

    line_data_->max_element = max_lines;
    line_data_->max_vertices = max_lines * line_data_->kVertexForSingleLine;
    
    // Allocating the memory for vertex Buffer Pointer
    line_data_->vertex_buffer_base_ptr = new LineData::Vertex[line_data_->max_vertices];
    
    // Create Pipeline instance
    line_data_->pipeline = Pipeline::Create();
    
    // Create vertes Buffer
    line_data_->vertex_buffer = VertexBuffer::Create(line_data_->max_vertices * sizeof(LineData::Vertex));
    line_data_->vertex_buffer->AddLayout({
      { "a_Position",     ShaderDataType::Float3 },
      { "a_Color",        ShaderDataType::Float4 },
    });
    line_data_->pipeline->AddVertexBuffer(line_data_->vertex_buffer);
    
    // Setup the Lines Shader
    line_data_->shader = Renderer::GetShader(AM::CoreAsset("shaders/batch_line_shader.glsl"));
    
    Renderer2DStats::Get().max_lines = line_data_->max_element;
    
    IK_CORE_INFO(LogModule::Batch2DRenderer, "Initialized Batch Renderer for Line Data");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max Lines per Batch             | {0}", max_lines);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used              | {0} B ({1} KB) ",
                 line_data_->max_vertices * sizeof(LineData::Vertex),
                 line_data_->max_vertices * sizeof(LineData::Vertex) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Shader used                     | {0}", line_data_->shader->GetName());
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
  }
  
  void BatchRenderer::BeginBatch(const glm::mat4& camera_view_projection_matrix) {
    // ----------------------------------------------------------------------
    // Start batch for quads
    // ----------------------------------------------------------------------
    if (quad_data_) {
      quad_data_->environment.camera_view_projection_matrix = camera_view_projection_matrix;
      
      quad_data_->shader->Bind();
      quad_data_->shader->SetUniformMat4("u_ViewProjection", camera_view_projection_matrix);
      
      quad_data_->StartBatch();
      quad_data_->shader->Unbind();
    }
    // ----------------------------------------------------------------------
    // Start batch for circles
    // ----------------------------------------------------------------------
    if (circle_data_) {
      circle_data_->environment.camera_view_projection_matrix = camera_view_projection_matrix;
      
      circle_data_->shader->Bind();
      circle_data_->shader->SetUniformMat4("u_ViewProjection", camera_view_projection_matrix);
      circle_data_->StartBatch();
      circle_data_->shader->Unbind();
    }
    // ----------------------------------------------------------------------
    // Start batch for lines
    // ----------------------------------------------------------------------
    if (line_data_) {
      line_data_->shader->Bind();
      line_data_->shader->SetUniformMat4("u_ViewProjection", camera_view_projection_matrix);
      line_data_->StartBatch();
      line_data_->shader->Unbind();
    }
  }
  
  void BatchRenderer::EndBatch() {
    Flush();
  }
  
  void BatchRenderer::Flush() {
    if (quad_data_ and quad_data_->index_count) {
      uint32_t data_size = (uint32_t)((uint8_t*)quad_data_->vertex_buffer_ptr -
                                      (uint8_t*)quad_data_->vertex_buffer_base_ptr);
      quad_data_->vertex_buffer->SetData(quad_data_->vertex_buffer_base_ptr, data_size);
      
      // Bind the shader
      quad_data_->shader->Bind();
      
      // Bind textures
      for (uint32_t i = 0; i < quad_data_->texture_slot_index; i++)
        quad_data_->texture_slots[i]->Bind(i);
      
      // Render the Scene
      Renderer::DrawIndexed(quad_data_->pipeline, quad_data_->index_count);
    }
    
    if (circle_data_ and circle_data_->index_count) {
      uint32_t dataSize = (uint32_t)((uint8_t*)circle_data_->vertex_buffer_ptr -
                                     (uint8_t*)circle_data_->vertex_buffer_base_ptr);
      circle_data_->vertex_buffer->SetData(circle_data_->vertex_buffer_base_ptr, dataSize);
      
      // Bind the shader
      circle_data_->shader->Bind();
      
      // Bind textures
      for (size_t i = 0; i < circle_data_->texture_slot_index; i++)
        circle_data_->texture_slots[i]->Bind((uint32_t)i);
      
      // Render the Scene
      Renderer::DrawIndexed(circle_data_->pipeline, circle_data_->index_count);
    }
    
    if (line_data_ and line_data_->vertex_count) {
      uint32_t dataSize = (uint32_t)((uint8_t*)line_data_->vertex_buffer_ptr -
                                     (uint8_t*)line_data_->vertex_buffer_base_ptr);
      line_data_->vertex_buffer->SetData(line_data_->vertex_buffer_base_ptr, dataSize);
      
      // Bind the shader
      line_data_->shader->Bind();
      
      // Render the Scene
      Renderer::DrawLines(line_data_->pipeline, line_data_->vertex_count);
    }
  }
  
  void BatchRenderer::NextBatch() {
    EndBatch();
    if (quad_data_) quad_data_->StartBatch();
    if (circle_data_) circle_data_->StartBatch();
    if (line_data_) line_data_->StartBatch();
  }
  
  uint32_t BatchRenderer::MaxQuads() {
    if (!quad_data_) return 0;
    return quad_data_->max_element;
  }

  uint32_t BatchRenderer::MaxCircles() {
    if (!circle_data_) return 0;
    return circle_data_->max_element;
  }

  uint32_t BatchRenderer::MaxLines() {
    if (!line_data_) return 0;
    return line_data_->max_element;
  }
  
  void BatchRenderer::DrawQuad(const glm::mat4& transform,
                               const glm::vec4& color,
                               int32_t object_id) {
    DrawTextureQuad(transform,
                    nullptr,
                    texture_coords_,
                    1.0f, //  tiling factor
                    color,
                    object_id);
  }
  
  void BatchRenderer::DrawQuad(const glm::mat4& transform,
                               const std::shared_ptr<Texture>& texture,
                               const glm::vec4& tint_color,
                               float tiling_factor,
                               int32_t object_id) {
    DrawTextureQuad(transform,
                    texture ? texture : nullptr,
                    texture_coords_,
                    tiling_factor,
                    tint_color,
                    object_id
                    );
  }

  void BatchRenderer::DrawQuad(const glm::mat4& transform,
                               const std::shared_ptr<SubTexture>& sub_texture,
                               const glm::vec4& tint_color,
                               int32_t object_id) {
    const glm::vec2* texture_coords = sub_texture->GetTexCoord();
    DrawTextureQuad(transform,
                    (sub_texture) ? sub_texture->GetSpriteImage() : nullptr,
                    (sub_texture) ? texture_coords : texture_coords_,
                    1.0f,
                    tint_color,
                    object_id);
  }

  void BatchRenderer::DrawTextureQuad(const glm::mat4& transform,
                                      const std::shared_ptr<Texture>& texture,
                                      const glm::vec2* texture_coords,
                                      float tiling_factor,
                                      const glm::vec4& tint_color,
                                      int32_t object_id) {
    // If number of indices increase in batch then start new batch
    if (quad_data_->index_count >= quad_data_->max_indices) {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Starts the new batch as number of indices ({0}) increases "
                   "in the previous batch", quad_data_->index_count);
      NextBatch();
    }
    
    float texture_index = 0.0f;
    if (texture) {
      // Find if texture is already loaded in current batch
      for (size_t i = 1; i < quad_data_->texture_slot_index; i++) {
        if (quad_data_->texture_slots[i].get() == texture.get()) {
          // Found the current textue in the batch
          texture_index = (float)i;
          break;
        }
      }
      
      // If current texture slot is not pre loaded then load the texture in
      // proper slot
      if (texture_index == 0.0f) {
        // If number of slots increases max then start new batch
        if (quad_data_->texture_slot_index >= kMaxTextureSlotsInShader) {
          IK_CORE_WARN(LogModule::Batch2DRenderer, "Starts the new batch as number of texture slot ({0}) "
                       "increases in the previous batch",
                       quad_data_->texture_slot_index);
          NextBatch();
        }
        
        // Loading the current texture in the first free slot slot
        texture_index = (float)quad_data_->texture_slot_index;
        quad_data_->texture_slots[quad_data_->texture_slot_index] = texture;
        quad_data_->texture_slot_index++;
      }
    }
    
    for (size_t i = 0; i < BatchRendererData::VertexForSingleElement; i++) {
      quad_data_->vertex_buffer_ptr->position         = transform * quad_data_->vertex_base_position[i];
      quad_data_->vertex_buffer_ptr->color            = tint_color;
      quad_data_->vertex_buffer_ptr->texture_coords   = texture_coords[i];
      quad_data_->vertex_buffer_ptr->texture_index    = texture_index;
      quad_data_->vertex_buffer_ptr->tiling_factor    = tiling_factor;
      quad_data_->vertex_buffer_ptr->object_id        = object_id;
      quad_data_->vertex_buffer_ptr++;
    }
    
    quad_data_->index_count += BatchRendererData::IndicesForSingleElement;
    
    RendererStatistics::Get().index_count += BatchRendererData::IndicesForSingleElement;
    RendererStatistics::Get().vertex_count += BatchRendererData::VertexForSingleElement;
    
    Renderer2DStats::Get().quads++;
  }
  
  void BatchRenderer::DrawCircle(const glm::vec3& position,
                                 const glm::vec3& radius,
                                 const glm::vec3& rotation,
                                 const glm::vec4& color,
                                 float thickness,
                                 float fade,
                                 int32_t object_id) {
    auto transform  = Math::GetTransformMatrix(position, rotation, radius);
    DrawTextureCircle(transform,
                      nullptr,
                      1.0f, // tiling factor
                      color,
                      thickness,
                      fade,
                      object_id);
  }


  void BatchRenderer::DrawCircle(const glm::mat4& transform,
                                 const glm::vec4& color,
                                 float thickness,
                                 float fade,
                                 int32_t object_id) {
    DrawTextureCircle(transform,
                      nullptr,
                      1.0f, // tiling factor
                      color,
                      thickness,
                      fade,
                      object_id
                      );
  }
  void BatchRenderer::DrawCircle(const glm::mat4& transform,
                                 const std::shared_ptr<Texture>& texture,
                                 const glm::vec4& tint_color,
                                 float tiling_factor,
                                 float thickness,
                                 float fade,
                                 int32_t object_id){
    DrawTextureCircle(transform,
                      texture,
                      tiling_factor,
                      tint_color,
                      thickness,
                      fade,
                      object_id
                      );
  }
  
  void BatchRenderer::DrawTextureCircle(const glm::mat4& transform,
                                        const std::shared_ptr<Texture>& texture,
                                        float tiling_factor,
                                        const glm::vec4& tint_color,
                                        float thickness,
                                        float fade,
                                        int32_t object_id) {
    // If number of indices increase in batch then start new batch
    if (circle_data_->index_count >= circle_data_->max_indices) {
      IK_CORE_WARN(LogModule::Batch2DRenderer, "Starts the new batch as number of indices ({0}) increases "
                   "in the previous batch", circle_data_->index_count);
      NextBatch();
    }
    
    float texture_index = 0.0f;
    if (texture) {
      // Find if texture is already loaded in current batch
      for (size_t i = 1; i < circle_data_->texture_slot_index; i++) {
        if (circle_data_->texture_slots[i].get() == texture.get()) {
          // Found the current textue in the batch
          texture_index = (float)i;
          break;
        }
      }
      
      // If current texture slot is not pre loaded then load the texture in proper slot
      if (texture_index == 0.0f) {
        // If number of slots increases max then start new batch
        if (circle_data_->texture_slot_index >= kMaxTextureSlotsInShader) {
          IK_CORE_WARN(LogModule::Batch2DRenderer, "Starts the new batch as number of texture slot ({0}) "
                       "increases in the previous batch",
                       circle_data_->texture_slot_index);
          NextBatch();
        }
        
        // Loading the current texture in the first free slot slot
        texture_index = (float)circle_data_->texture_slot_index;
        circle_data_->texture_slots[circle_data_->texture_slot_index] = texture;
        circle_data_->texture_slot_index++;
      }
    }
    
    for (size_t i = 0; i < BatchRendererData::VertexForSingleElement; i++) {
      circle_data_->vertex_buffer_ptr->position         = transform * circle_data_->vertex_base_position[i];
      circle_data_->vertex_buffer_ptr->color            = tint_color;
      circle_data_->vertex_buffer_ptr->texture_coords   = 2.0f * circle_data_->vertex_base_position[i];
      circle_data_->vertex_buffer_ptr->texture_index    = texture_index;
      circle_data_->vertex_buffer_ptr->tiling_factor    = tiling_factor;
      circle_data_->vertex_buffer_ptr->local_position   = 2.0f * circle_data_->vertex_base_position[i];
      circle_data_->vertex_buffer_ptr->thickness        = thickness;
      circle_data_->vertex_buffer_ptr->fade             = fade;
      circle_data_->vertex_buffer_ptr->object_id        = object_id;
      circle_data_->vertex_buffer_ptr++;
    }
    
    circle_data_->index_count += BatchRendererData::IndicesForSingleElement;
    
    RendererStatistics::Get().index_count += BatchRendererData::IndicesForSingleElement;
    RendererStatistics::Get().vertex_count += BatchRendererData::VertexForSingleElement;
    
    Renderer2DStats::Get().circles++;
  }
  
  void BatchRenderer::DrawLine(const glm::vec3& p0,
                               const glm::vec3& p1,
                               const glm::vec4& color) {
    line_data_->vertex_buffer_ptr->position = p0;
    line_data_->vertex_buffer_ptr->color = color;
    line_data_->vertex_buffer_ptr++;
    
    line_data_->vertex_buffer_ptr->position = p1;
    line_data_->vertex_buffer_ptr->color = color;
    line_data_->vertex_buffer_ptr++;
    
    line_data_->vertex_count += LineData::kVertexForSingleLine;
    RendererStatistics::Get().vertex_count += LineData::kVertexForSingleLine;
    
    Renderer2DStats::Get().lines++;
  }
  
  void BatchRenderer::DrawRect(const glm::vec3& p0, const glm::vec3& p2, const glm::vec4& color) {
    glm::vec3 p1 = glm::vec3(p2.x, p0.y, p0.z);
    glm::vec3 p3 = glm::vec3(p0.x, p2.y, p2.z);

    DrawLine(p0, p1, color);
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p0, color);
  }

  void BatchRenderer::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
  {
    glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
    glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);
    
    DrawLine(p0, p1, color);
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p0, color);
  }
  
  void BatchRenderer::DrawRect(const glm::mat4& transform, const glm::vec4& color)
  {
    glm::vec3 line_vertices[4];
    for (size_t i = 0; i < 4; i++)
      line_vertices[i] = transform * quad_data_->vertex_base_position[i];
    
    DrawLine(line_vertices[0], line_vertices[1], color);
    DrawLine(line_vertices[1], line_vertices[2], color);
    DrawLine(line_vertices[2], line_vertices[3], color);
    DrawLine(line_vertices[3], line_vertices[0], color);
  }

  void BatchRenderer::DrawRect(const AABB& aabb, const glm::vec4& color)
  {
    glm::vec3 p0 = glm::vec3(0.0f + aabb.min.x, 0.0f + aabb.min.y, 0.0f);
    glm::vec3 p1 = glm::vec3(0.0f + aabb.max.x, 0.0f + aabb.min.y, 0.0f);
    glm::vec3 p2 = glm::vec3(0.0f + aabb.max.x, 0.0f + aabb.max.y, 0.0f);
    glm::vec3 p3 = glm::vec3(0.0f + aabb.min.x, 0.0f + aabb.max.y, 0.0f);
    
    DrawLine(p0, p1, color);
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p0, color);
  }
  
}
