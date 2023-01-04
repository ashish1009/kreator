//
//  chess_main.cpp
//  chess
//
//  Created by Ashish . on 02/01/23.
//

#include "chess_main.hpp"

namespace chess {
  
  ChessLayer::ChessLayer() : Layer("Kreator") {
    IK_INFO("Chess", "Creating Chess Layer instance ... ");
  }
  
  ChessLayer::~ChessLayer() {
    IK_WARN("Chess", "Destroying Chess Layer instance !!! ");
  }
  
  void ChessLayer::Attach() {
    IK_INFO("Chess", "Attaching Chess Layer instance");
  }
  
  void ChessLayer::Detach() {
    IK_WARN("Chess", "Detaching Chess Layer instance ");
  }
  
  void ChessLayer::Update(Timestep ts) {
  }
  
  void ChessLayer::EventHandler(Event& event) {
  }
  
  void ChessLayer::RenderGui() {
    ImguiAPI::StartDcocking();
    Renderer::Framerate();
    Renderer::RenderStatsGui();
    
    ImguiAPI::EndDcocking();
  }
  
}
