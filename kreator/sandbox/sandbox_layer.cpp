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
  }
  
  void SandboxLayer::Detach() {
    IK_WARN("Detaching Sandbox Layer instance ");
  }
  
  void SandboxLayer::Update(Timestep ts) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1, 0.1, 0.1, 1.0);
  }
  
  void SandboxLayer::EventHandler(Event& event) {
  }
  
  void SandboxLayer::RenderGui() {
    Renderer::Framerate();
  }
  
} // namespace sandbox
