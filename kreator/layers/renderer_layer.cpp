//
//  renderer_layer.cpp
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#include "renderer_layer.hpp"

namespace kreator {
  
  RendererLayer::RendererLayer(GameType game_type) : Layer("ikan Game"), game_data_(CreateGameData(game_type)) {
  }
  
  RendererLayer::~RendererLayer() {
  }
  
  void RendererLayer::Attach() {
  }
  
  void RendererLayer::Detach() {
  }
  
  void RendererLayer::Update(Timestep ts) {
  }
  
  void RendererLayer::EventHandler(Event& event) {
  }
  
  void RendererLayer::RenderGui() {
  }
  
}
