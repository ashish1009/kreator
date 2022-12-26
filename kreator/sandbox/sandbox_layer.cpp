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
    
    std::shared_ptr<Shader> test_shader = Renderer::GetShader(AM::CoreAsset("shaders/pbr_static_shader.glsl"));
    test_shader = Renderer::GetShader(AM::CoreAsset("shaders/batch_quad_shader.glsl"));
  }
  
  void SandboxLayer::Detach() {
    IK_WARN("Detaching Sandbox Layer instance ");
  }
  
  void SandboxLayer::Update(Timestep ts) {
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
  }
  
  void SandboxLayer::EventHandler(Event& event) {
  }
  
  void SandboxLayer::RenderGui() {
    Renderer::Framerate();
  }
  
} // namespace sandbox
