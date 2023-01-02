//
//  mario_main.cpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#include "mario_main.hpp"

namespace mario {
  
  MarioLayer::MarioLayer() : Layer("Kreator") {
    IK_INFO("Creating Mario Layer instance ... ");
  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Destroying Mario Layer instance !!! ");
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Attaching Mario Layer instance");
  }
  
  void MarioLayer::Detach() {
    IK_WARN("Detaching Mario Layer instance ");
  }
  
  void MarioLayer::Update(Timestep ts) {
  }
  
  void MarioLayer::EventHandler(Event& event) {
  }
  
  void MarioLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    
    ImguiAPI::EndDcocking();
  }
  
}
