//
//  aabb_renderer.cpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#include "aabb_renderer.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/graphics/shader.hpp"
#include "renderer/utils/renderer_stats.hpp"

namespace ikan {
  
  /// Stores the renderering data for box
  struct AABBRendererData {
    /// Vertex of 3D Box Data
    struct Vertex {
      glm::vec3 Position;
    };
    
    static constexpr uint32_t max_box_per_batch = 1000;
    
    std::shared_ptr<Pipeline> pipeline;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::shared_ptr<Shader> shader;
    
    // Pointers attribute of vertex
    Vertex* vertex_buffer_base = nullptr;
    Vertex* vertex_buffer_ptr = nullptr;
    
    uint32_t num_cubes = 0;
    
    /// start new batch for quad rendering
    void StartBatch() {
      vertex_buffer_ptr = vertex_buffer_base;
      num_cubes = 0;
    }
  };
  static AABBRendererData* s_data;

  /// Initialize the 3d Box Data
  void AABBRenderer::Init() {
    s_data = new AABBRendererData();
    
    // Create Vertex Array
    s_data->pipeline = Pipeline::Create();
    
    // Create vertes Buffer
    s_data->vertex_buffer = VertexBuffer::Create(36 * s_data->max_box_per_batch * sizeof(AABBRendererData::Vertex));
    s_data->vertex_buffer->AddLayout({
      { "a_Position", ShaderDataType::Float3 },
    });
    s_data->pipeline->AddVertexBuffer(s_data->vertex_buffer);
    
    s_data->vertex_buffer_base = new AABBRendererData::Vertex[36 * s_data->max_box_per_batch];
    
    // Setup the 3d Box Shader
    s_data->shader = Renderer::GetShader(AM::CoreAsset("shaders/aabb_shader.glsl"));
    
    IK_CORE_INFO(LogModule::Batch2DRenderer, "Initialized AABB Renderer");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Max AABB per Batch            | {0}", s_data->max_box_per_batch);
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used            | {0} B ({1} KB) ",
                 36 * sizeof(AABBRendererData::Vertex), 36 * sizeof(AABBRendererData::Vertex) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Vertex Buffer used            | {0} B ({1} KB) ",
                 36 * sizeof(uint32_t), 36 * sizeof(uint32_t) / 1000.0f );
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  Shader used                   | {0}", s_data->shader->GetName());
    IK_CORE_INFO(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
  }
  
  void AABBRenderer::Shutdown() {
    IK_CORE_WARN(LogModule::Batch2DRenderer, "Destrying AABB Renderer");
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  Max AABB per Batch            | {0}", s_data->max_box_per_batch);
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used            | {0} B ({1} KB) ",
                 36 * sizeof(AABBRendererData::Vertex), 36 * sizeof(AABBRendererData::Vertex) / 1000.0f );
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  Vertex Buffer used            | {0} B ({1} KB) ",
                 36 * sizeof(uint32_t), 36 * sizeof(uint32_t) / 1000.0f );
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  Shader used                   | {0}", s_data->shader->GetName());
    IK_CORE_WARN(LogModule::Batch2DRenderer, "  ---------------------------------------------------------");

    delete s_data;
  }
  
  void AABBRenderer::BeginRenderer(const glm::mat4& view_projection) {
    s_data->shader->Bind();
    s_data->shader->SetUniformMat4("u_ViewProjection", view_projection);
    s_data->StartBatch();
    Renderer::BeginWireframe();
  }
  
  void AABBRenderer::EndRenderer() {
    uint32_t data_size = (uint32_t)((uint8_t*)s_data->vertex_buffer_ptr - (uint8_t*)s_data->vertex_buffer_base);
    s_data->vertex_buffer->SetData(s_data->vertex_buffer_base, data_size);
    
    Renderer::DrawCube(s_data->pipeline, s_data->num_cubes);
    Renderer::EndWireframe();
  }

  void AABBRenderer::DrawCube(const AABB& aabb) {
    Draw(aabb);
  }

  void AABBRenderer::DrawQuad(const AABB& aabb) {
    Draw(AABB({aabb.min.x, aabb.min.y, 0.0f}, {aabb.max.x, aabb.max.y, 0.0f}));
  }
  
  void AABBRenderer::Draw(const AABB& aabb) {
    // Flush the Scene
    if (s_data->num_cubes == s_data->max_box_per_batch) {
      EndRenderer();
      
      s_data->StartBatch();
      Renderer::BeginWireframe();
    }
    
    s_data->vertex_buffer_ptr[0].Position  = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[1].Position  = glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[2].Position  = glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[3].Position  = glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[4].Position  = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[5].Position  = glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0f );
    
    s_data->vertex_buffer_ptr[6].Position  = glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[7].Position  = glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[8].Position  = glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[9].Position  = glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[10].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[11].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0f );
    
    s_data->vertex_buffer_ptr[12].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[13].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[14].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[15].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[16].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[17].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0f );
    
    s_data->vertex_buffer_ptr[18].Position = glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[19].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[20].Position = glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[21].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[22].Position = glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[23].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0f );
    
    s_data->vertex_buffer_ptr[24].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[25].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[26].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[27].Position = glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[28].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[29].Position = glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f );
    
    s_data->vertex_buffer_ptr[30].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[31].Position = glm::vec4(aabb.max.x, aabb.max.y ,aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[32].Position = glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[33].Position = glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f );
    s_data->vertex_buffer_ptr[34].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0f );
    s_data->vertex_buffer_ptr[35].Position = glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0f );
    
    s_data->vertex_buffer_ptr+=36;
    RendererStatistics::Get().vertex_count += 36;
    s_data->num_cubes++;
  }

}
