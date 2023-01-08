//
//  aabb_renderer.cpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#include "aabb_renderer.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/graphics/shader.hpp"

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
  
  
  
}
