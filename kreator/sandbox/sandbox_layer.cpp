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
    
    float data[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f
    };
    
    vertex_buffer_ = VertexBuffer::Create(data, sizeof(data));
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
