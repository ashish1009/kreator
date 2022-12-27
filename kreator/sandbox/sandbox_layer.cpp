//
//  sandbox_layer.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "sandbox_layer.hpp"
#include <glad/glad.h>

namespace sandbox {
    
  SandboxLayer::SandboxLayer() : Layer("Sandbox") {
    IK_INFO("Creating Sandbox Layer instance ... ");
  }
  
  SandboxLayer::~SandboxLayer() {
    IK_WARN("Destroying Sandbox Layer instance !!! ");
  }
  
  void SandboxLayer::Attach() {
    IK_INFO("Attaching Sandbox Layer instance");
    basic_shader_ = Renderer::GetShader(AM::CoreAsset("shaders/basic_shader.glsl"));
    
    pipeline_ = Pipeline::Create();
    
    float vertex_data[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f
    };
    vertex_buffer_ = VertexBuffer::Create(vertex_data, sizeof(vertex_data));
    vertex_buffer_->AddLayout({
      { "a_Position",     ShaderDataType::Float3 },
    });
    pipeline_->AddVertexBuffer(vertex_buffer_);

    uint32_t indices_data[] = {0, 1, 2, 2, 3, 0};
    index_buffer_ = IndexBuffer::CreateWithCount(indices_data, 6);
    pipeline_->SetIndexBuffer(index_buffer_);
  }
  
  void SandboxLayer::Detach() {
    IK_WARN("Detaching Sandbox Layer instance ");
  }
  
  void SandboxLayer::Update(Timestep ts) {
    Renderer::ResetStatsEachFrame();
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
  }
  
  void SandboxLayer::EventHandler(Event& event) {
  }
  
  void SandboxLayer::RenderGui() {
    Renderer::Framerate();
    Renderer::RenderStatsGui(false);
  }
  
} // namespace sandbox
