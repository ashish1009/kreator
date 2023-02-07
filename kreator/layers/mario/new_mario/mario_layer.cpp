//
//  mario_layer.cpp
//  kreator
//
//  Created by Ashish . on 07/02/23.
//

#include "mario_layer.hpp"

namespace mario {
  
  RendererLayer::RendererLayer() : Layer("Mario") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
  }
    
  void RendererLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
  }
  
  void RendererLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
  }
    
  void RendererLayer::Update(Timestep ts) {
  }
  
  void RendererLayer::EventHandler(Event& event) {
  }
  
  void RendererLayer::RenderGui() {
  }
  
}
