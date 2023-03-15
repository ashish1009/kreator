//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#include "renderer_layer.hpp"

namespace kreator {
  
  RendererLayer::RendererLayer(GameType game_type) : Layer("Kreator"), game_data_(CreateGameData(game_type)) {
    IK_INFO(game_data_->GameName(), "Creating {0} Layer instance ... ", game_data_->GameName().c_str());
  }
  
  RendererLayer::~RendererLayer() {
    IK_WARN(game_data_->GameName(), "Destroying {0} Layer instance !!! ", game_data_->GameName().c_str());
  }
  
  void RendererLayer::Attach() {
    IK_INFO(game_data_->GameName(), "Attaching {0} Layer instance", game_data_->GameName().c_str());
  }
  
  void RendererLayer::Detach() {
    IK_WARN(game_data_->GameName(), "Detaching {0} Layer instance ", game_data_->GameName().c_str());
  }
  
  void RendererLayer::Update(Timestep ts) {
  }
  
  void RendererLayer::EventHandler(Event& event) {
  }
  
  void RendererLayer::RenderGui() {
    if (setting_.play) {
      
    }
    else {
      ImguiAPI::StartDcocking();
      ImguiAPI::EndDcocking();
    }
  }
  
}
