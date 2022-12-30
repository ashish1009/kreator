//
//  ray_tracing_layer.hpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#pragma once

namespace ray_tracing {
  
  using namespace ikan;

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
  };
  
  struct Sphere {
    glm::vec3 position;
    float radius = 0.5f;
    
    glm::vec3 albedo;
    
    Sphere(const glm::vec3& position, float radius, const glm::vec3& albedo)
    : position(position), radius(radius), albedo(albedo) {
      
    }
  };
  
  class RayScene {
  public:
    std::vector<Sphere> shperes;
  };

  class RayTracingLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    RayTracingLayer();
    /// Default Destructor
    ~ RayTracingLayer();
    
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
    void Resize();
    void Render();
    glm::vec4 TraceRay(const Ray& ray);
    
    std::shared_ptr<Image> final_image_ = nullptr;
    uint32_t* image_data_ = nullptr;
    uint32_t viewport_width_ = 900, viewport_height_ = 600;
    EditorCamera editor_camera_;
    RayScene scene_;
  };
  
}
