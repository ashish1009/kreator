//
//  editor_layer.cpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  glm::vec3 p1 = {0, 0, 0};
  glm::vec3 p2 = {0, -5, 0};
  
  glm::vec3 cp1 = {0, 0, 40};
  
  glm::vec3 r1 = {0, 0, 0};
  glm::vec3 r2 = {0, 0, 0};
  
  glm::vec3 s1 = {1, 1, 1};
  glm::vec3 s2 = {10, 1, 1};
  
  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Editor", "Creating Editor Layer instance ... ");
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Editor", "Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Editor", "Attaching Editor Layer instance");
    camera_.SetProjectionType(SceneCamera::ProjectionType::Perspective);
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Editor", "Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    Renderer::Clear({0.2, 0.2, 0.2, 1.0});
    
    BatchRenderer::BeginBatch(camera_.GetProjection() * glm::inverse(Math::GetTransformMatrix(cp1, {0, 0, 0}, {1, 1, 1})));
    BatchRenderer::DrawQuad(Math::GetTransformMatrix(p1, r1, s1), {1, 0, 0, 1});
    BatchRenderer::DrawQuad(Math::GetTransformMatrix(p2, r2, s2), {1, 0, 1, 1});
    BatchRenderer::EndBatch();
  }
  
  void EditorLayer::EventHandler(Event& event) {
  }

  void EditorLayer::RenderGui() {
    ImGui::Begin("Camera");
    camera_.RenderGui();
    PropertyGrid::Float3("CameraPos", cp1);
    ImGui::End();
  }
} 
