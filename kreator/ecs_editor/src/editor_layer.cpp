//
//  editor_layer.cpp
//  kreator
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Creating Editor Layer instance ... ");
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Attaching Editor Layer instance");
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    Renderer::Clear({0.1, 0.1, 0.1, 1.0});
  }
  
  void EditorLayer::EventHandler(Event& event) {
  }
    
  void EditorLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    ImguiAPI::EndDcocking();
  }
  
} // namespace sandbox
