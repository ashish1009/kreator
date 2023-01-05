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
    IK_INFO("Sandbox", "Creating Sandbox Layer instance ... ");
  }
  
  SandboxLayer::~SandboxLayer() {
    IK_WARN("Sandbox", "Destroying Sandbox Layer instance !!! ");
  }
  
  void SandboxLayer::Attach() {
    IK_INFO("Sandbox", "Attaching Sandbox Layer instance");
    checkboard_ = Renderer::GetTexture(AM::ClientAsset("textures/checkerboard.png"));
    TextRenderer::LoadFreetype(AM::ClientAsset("fonts/opensans/OpenSans-Bold.ttf"));

  }
  
  void SandboxLayer::Detach() {
    IK_WARN("Sandbox", "Detaching Sandbox Layer instance ");
  }
  
  void SandboxLayer::Update(Timestep ts) {
    glm::mat4 still_camera_projection =  glm::ortho( 0.0f,
                                                    (float)editor_camera.GetViewportWidth(),
                                                    0.0f,
                                                    (float)editor_camera.GetViewportHeight());

    editor_camera.Update(ts);
    
    Renderer::ResetStatsEachFrame();
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
    
    BatchRenderer::BeginBatch(editor_camera.GetViewProjection(), editor_camera.GetView());
    BatchRenderer::DrawCircle(Math::GetTransformMatrix({0, 0, 0},
                                                       {0, 0, 0},
                                                       {1, 1, 0}),
                              checkboard_,
                              {0.2, 0.4, 0.6, 1.0});
    BatchRenderer::DrawQuad(Math::GetTransformMatrix({-3, 0, 0},
                                                     {0, 0, 0},
                                                     {1, 1, 0}),
                            checkboard_,
                            {0.2, 0.4, 0.6, 1.0});
    BatchRenderer::DrawLine({-1000, 0, 0}, {1000, 0, 0}, {0.8, 0.5, 0.3, 1.0});
    BatchRenderer::EndBatch();
    
    TextRenderer::RenderText(std::to_string((uint32_t)(ImGui::GetIO().Framerate)),
                             still_camera_projection,
                             { 5.0f, 5.0f, 0.3f },
                             { 0.25f, 0.25f },
                             { 0.1f, 0.1f, 0.1f, 1.0f });
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
    Renderer::RenderStatsGui(nullptr, false);
  }
  
} // namespace sandbox
