//
//  editor_layer.hpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#pragma once

namespace editor {
  
  using namespace ikan;
  
  class EditorLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    EditorLayer();
    /// Default Destructor
    ~ EditorLayer();
    
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
    Viewport viewport_;
    EnttScene active_scene_;
    ContentBrowserPanel cbp_;
  };
  
}
