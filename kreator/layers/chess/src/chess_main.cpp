//
//  chess_main.cpp
//  chess
//
//  Created by Ashish . on 02/01/23.
//

#include "chess_main.hpp"

namespace chess {
  
  ChessLayer::ChessLayer() : Layer("Kreator") {
    IK_INFO("Creating Chess Layer instance ... ");
  }
  
  ChessLayer::~ChessLayer() {
    IK_WARN("Destroying Chess Layer instance !!! ");
  }
  
  void ChessLayer::Attach() {
    IK_INFO("Attaching Chess Layer instance");
  }
  
  void ChessLayer::Detach() {
    IK_WARN("Detaching Chess Layer instance ");
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
