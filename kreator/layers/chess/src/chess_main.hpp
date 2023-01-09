//
//  chess_main.hpp
//  chess
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

#include "piece.hpp"
#include "block.hpp"

namespace chess {
  
  using namespace ikan;
  
  class ChessLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    ChessLayer();
    /// Default Destructor
    ~ChessLayer();
    
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    void Attach() override;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    void Detach() override;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// NOTE Core Application is taking care to call the RenderGui API for all Layers
    void RenderGui() override;
    /// This function updates the layer data. To be called each frame from application. NOTE Core
    /// Application is taking care to call the Update(ts) API for all Layers
    /// - Parameter ts: Time step between 2 Frames
    void Update(Timestep ts) override;
    /// This function handles all the events triggered for the layer. To be whenever an event is triggered
    /// in the window. NOTE Core Application is taking care to call the EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void EventHandler(Event& event) override;
  private:
    // --------------------
    // Member functions
    // --------------------
    /// This function handles the window resize
    /// - Parameter e: window resize event
    bool OnWindowResized(WindowResizeEvent& e);
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    
    // --------------------
    // Member variables
    // --------------------
    uint32_t viewport_width_ = 0, viewport_height_ = 0;
    EnttScene chess_scene_;
    Entity camera_entity_;
    
    // Cbess Data
    Block block_[MaxRows][MaxCols];
    Block* selected_block_ = nullptr;
        
    Viewport viewport_ = Viewport({ 0.2, 0.2, 0.2, 1.0f });

    // For Debug
#if CHESS_DEBUG
    ScenePanelManager spm_;
#endif
  };
  
}
