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
  
  /// Maximum Texture slot supported by Senderer Shader Current Open GL Version 4.1 supports
  /// only 16 Texture slot in Shader
  /// TODO: Change this value based on the Open GL Version
  static constexpr uint32_t kMaxTextureSlotsInShader = 16;
  
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
      glm::mat4 camera_view_matrix;
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
      IK_CORE_TRACE("Creating QuadData instance ...");
    }
    /// Destructor
    virtual ~QuadData() {
      IK_CORE_WARN("Destroying QuadData instance and clearing the data !!!");
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
      IK_CORE_TRACE("Creating CircleData instance ...");
    }
    /// Destructir
    virtual ~CircleData() {
      IK_CORE_WARN("Destroying Circle Data instance and clearing the data !!!");
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
      IK_CORE_TRACE("Creating Line Data instance ...");
    }
    /// Destructir
    virtual ~LineData() {
      IK_CORE_WARN("Destroying Line Data instance and clearing the data !!!");
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
  void BatchRenderer::Init() {
    IK_CORE_INFO("Initialising the Batch Renderer 2D ...");
    InitQuadData();
    InitCircleData();
    InitLineData();
  }

  void BatchRenderer::Shutdown() {
    IK_CORE_WARN("Shutting Down the Batch Renderer 2D !!!");
    
    if (quad_data_) {
      IK_CORE_WARN("Destroying the Batch Renderer Quad Data");
      IK_CORE_WARN("  ---------------------------------------------------------");
      IK_CORE_WARN("  Max Quads per Batch             | {0}", quad_data_->max_element);
      IK_CORE_WARN("  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN("  Vertex Buffer used              | {0} B ({1} KB) ",
                   quad_data_->max_vertices * sizeof(QuadData::Vertex), quad_data_->max_vertices * sizeof(QuadData::Vertex) / 1000.0f );
      IK_CORE_WARN("  Index Buffer used               | {0} B ({1} KB) ",
                   quad_data_->max_indices * sizeof(uint32_t), quad_data_->max_indices * sizeof(uint32_t) / 1000.0f );
      IK_CORE_WARN("  Shader Used                     | {0}", quad_data_->shader->GetName());
      IK_CORE_WARN("  ---------------------------------------------------------");
      delete quad_data_;
    }

    if (circle_data_) {
      IK_CORE_WARN("Destroying the Batch Renderer Circle Data");
      IK_CORE_WARN("  ---------------------------------------------------------");
      IK_CORE_WARN("  Max Circles per Batch           | {0}", circle_data_->max_element);
      IK_CORE_WARN("  Max Texture Slots Batch         | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN("  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
      IK_CORE_WARN("  Vertex Buffer used              | {0} B ({1} KB) ",
                   circle_data_->max_vertices * sizeof(CircleData::Vertex),
                   circle_data_->max_vertices * sizeof(CircleData::Vertex) / 1000.0f );
      IK_CORE_WARN("  Vertex Buffer used              | {0} B ({1} KB) ",
                   circle_data_->max_indices * sizeof(uint32_t),
                   circle_data_->max_indices * sizeof(uint32_t) / 1000.0f );
      IK_CORE_WARN("  Shader used                     | {0}", circle_data_->shader->GetName());
      IK_CORE_WARN("  ---------------------------------------------------------");
      delete circle_data_;
    }
    if (line_data_) {
      IK_CORE_WARN("Destroying the Batch Renderer Line Data");
      IK_CORE_WARN("  ---------------------------------------------------------");
      IK_CORE_WARN("  Max Lines per Batch             | {0}", line_data_->max_element);
      IK_CORE_WARN("  Vertex Buffer used              | {0} B ({1} KB) ",
                   line_data_->max_vertices * sizeof(LineData::Vertex),
                   line_data_->max_vertices * sizeof(LineData::Vertex) / 1000.0f );
      IK_CORE_WARN("  Shader used                   x  | {0}", line_data_->shader->GetName());
      IK_CORE_WARN("  ---------------------------------------------------------");
      delete line_data_;
    }

  }
  
  void BatchRenderer::InitQuadData(uint32_t max_quads) {
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
    
    IK_CORE_INFO("Initialized Batch Renderer for Quad Data");
    IK_CORE_INFO("  ---------------------------------------------------------");
    IK_CORE_INFO("  Max Quads per Batch             | {0}", quad_data_->max_element);
    IK_CORE_INFO("  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
    IK_CORE_INFO("  Vertex Buffer used              | {0} B ({1} KB) ",
                 quad_data_->max_vertices * sizeof(QuadData::Vertex), quad_data_->max_vertices * sizeof(QuadData::Vertex) / 1000.0f );
    IK_CORE_INFO("  Index Buffer used               | {0} B ({1} KB) ",
                 quad_data_->max_indices * sizeof(uint32_t), quad_data_->max_indices * sizeof(uint32_t) / 1000.0f );
    IK_CORE_INFO("  Shader Used                     | {0}", quad_data_->shader->GetName());
    IK_CORE_INFO("  ---------------------------------------------------------");
  }
  
  void BatchRenderer::InitCircleData(uint32_t max_circles) {
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
    
    IK_CORE_INFO("Initialized Batch Renderer for Circle Data");
    IK_CORE_INFO("  ---------------------------------------------------------");
    IK_CORE_INFO("  Max Circle per Batch            | {0}", max_circles);
    IK_CORE_INFO("  Max Texture Slots per Batch     | {0}", kMaxTextureSlotsInShader);
    IK_CORE_INFO("  Vertex Buffer used              | {0} B ({1} KB) ",
                 circle_data_->max_vertices * sizeof(CircleData::Vertex), circle_data_->max_vertices * sizeof(CircleData::Vertex) / 1000.0f );
    IK_CORE_INFO("  Vertex Buffer used              | {0} B ({1} KB) ",
                 circle_data_->max_indices * sizeof(uint32_t), circle_data_->max_indices * sizeof(uint32_t) / 1000.0f );
    IK_CORE_INFO("  Shader used                     | {0}", circle_data_->shader->GetName());
    IK_CORE_INFO("  ---------------------------------------------------------");
  }
  
  void BatchRenderer::InitLineData(uint32_t max_lines) {
    // Allocate memory for Line Data
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
    
    IK_CORE_INFO("Initialized Batch Renderer for Line Data");
    IK_CORE_INFO("  ---------------------------------------------------------");
    IK_CORE_INFO("  Max Lines per Batch             | {0}", max_lines);
    IK_CORE_INFO("  Vertex Buffer used              | {0} B ({1} KB) ",
                 line_data_->max_vertices * sizeof(LineData::Vertex),
                 line_data_->max_vertices * sizeof(LineData::Vertex) / 1000.0f );
    IK_CORE_INFO("  Shader used                     | {0}", line_data_->shader->GetName());
    IK_CORE_INFO("  ---------------------------------------------------------");
  }
  
  void BatchRenderer::BeginBatch(const glm::mat4& camera_view_projection_matrix,
                                 const glm::mat4& camera_view_matrix) {
    // ----------------------------------------------------------------------
    // Start batch for quads
    // ----------------------------------------------------------------------
    quad_data_->environment.camera_view_projection_matrix = camera_view_projection_matrix;
    quad_data_->environment.camera_view_matrix = camera_view_matrix;
    
    quad_data_->shader->Bind();
    quad_data_->shader->SetUniformMat4("u_ViewProjection",
                                       camera_view_projection_matrix);
    
    quad_data_->StartBatch();
    quad_data_->shader->Unbind();
    
    // ----------------------------------------------------------------------
    // Start batch for circles
    // ----------------------------------------------------------------------
    circle_data_->environment.camera_view_projection_matrix = camera_view_projection_matrix;
    circle_data_->environment.camera_view_matrix = camera_view_matrix;
    
    circle_data_->shader->Bind();
    circle_data_->shader->SetUniformMat4("u_ViewProjection",
                                         camera_view_projection_matrix);
    circle_data_->StartBatch();
    circle_data_->shader->Unbind();
    
    // ----------------------------------------------------------------------
    // Start batch for lines
    // ----------------------------------------------------------------------
    line_data_->shader->Bind();
    line_data_->shader->SetUniformMat4("u_ViewProjection",
                                       camera_view_projection_matrix);
    line_data_->StartBatch();
    line_data_->shader->Unbind();
  }
  
}
