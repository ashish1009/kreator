//
//  layer.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the interface to create a Layer at Core or Client. All the
// layer that need to be stored in Application should be derived from this Layer

namespace ikan {
  
  // Forward declaration
  class Event;
  class Timestep;
  
  /// Layer Interface class Core Application manages Bellow API As per functionality defined
  class Layer {
  public:
    // ----------------------------
    // Constructors and Destructor
    // ----------------------------
    /// Layer Default Constructor to store the name of layer
    /// - Parameter name: name of layer
    Layer(const std::string& name) : name_(name) {}
    /// Default Destructor
    virtual ~Layer() = default;
    
    // -------------
    // Fundamentals
    // -------------
    /// This funciton initialises the layer, calls when we push layer in the application (To be called only once)
    virtual void Attach() = 0;
    /// This funciton destroy the layer, calls when we pop layer from the application (To be called only once)
    virtual void Detach() = 0;
    
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    /// NOTE Core ikan::Application is taking care to call the RenderGui API for all Layers
    virtual void RenderGui() = 0;
    /// This function updates the layer data. To be called each frame from application. NOTE Core
    /// ikan::Application is taking care to call the Update(ts) API for all Layers
    /// - Parameter ts: Time step between 2 Frames
    virtual void Update(Timestep ts) = 0;
    /// This function handles all the events triggered for the layer. To be whenever an event is
    /// triggered in the window NOTE Core ikan::Application is taking care to call the
    /// EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Dispatcher
    virtual void EventHandler(Event& event) = 0;
    
    /// This function returns the name of layer
    const std::string& GetName() const { return name_; };
    
  protected:
    std::string name_ = "Unknown";
  };
  
} // namesapce ikan
