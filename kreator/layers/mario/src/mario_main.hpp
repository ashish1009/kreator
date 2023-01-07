//
//  mario_main.hpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

#define MARIO_DEBUG 1

#include "background.hpp"
#include "player.hpp"

namespace mario {
  
  using namespace ikan;
  
  class MarioLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    MarioLayer();
    /// Default Destructor
    ~MarioLayer();
    
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
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

    EnttScene mario_scene_;
    Viewport viewport_ = Viewport({ 0.12, 0.23, 0.34, 1.0f });
    
    // Render Data
    Entity camera_entity_;
    BackgroudData* background_data_;
    
    Player* player_;
  
    // For Debug
#if MARIO_DEBUG
    ScenePanelManager spm_;
#endif
  };
  
}
