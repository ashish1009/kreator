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
    checkboard_ = Renderer::GetTexture(AM::ClientAsset("textures/checkerboard.png"));
  }
  
  void SandboxLayer::Detach() {
    IK_WARN("Detaching Sandbox Layer instance ");
  }
  
  void SandboxLayer::Update(Timestep ts) {
    editor_camera.Update(ts);
    
    Renderer::ResetStatsEachFrame();
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
    
    BatchRenderer::BeginBatch(editor_camera.GetViewProjection(), editor_camera.GetView());
    BatchRenderer::DrawCircle(glm::mat4(1.0f), checkboard_, {0.2, 0.4, 0.6, 1.0});
    BatchRenderer::DrawQuad(glm::mat4(12.0f), checkboard_, {0.2, 0.4, 0.6, 1.0});
    BatchRenderer::EndBatch();
  }
  
  void SandboxLayer::EventHandler(Event& event) {
    editor_camera.EventHandler(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(SandboxLayer::WindowResize));
  }
  
  bool SandboxLayer::WindowResize(WindowResizeEvent &window_resize_event) {
    editor_camera.SetViewportSize(window_resize_event.GetWidth(), window_resize_event.GetHeight());
    return false;
  }
  
  void SandboxLayer::RenderGui() {
    Renderer::Framerate();
    Renderer::RenderStatsGui(false);
  }
  
} // namespace sandbox
