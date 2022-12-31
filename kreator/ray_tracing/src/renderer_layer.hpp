//
//  renderer_layer.hpp
//  kreator
//
//  Created by Ashish . on 31/12/22.
//

#pragma once

#include "sphere.hpp"

namespace ray_tracing {
  
  using namespace ikan;
  
  class RendererLayer : public Layer {
  public:
    /// Layer Default Constructor to store the name of layer
    RendererLayer();
    /// Default Destructor
    ~ RendererLayer();
    
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
    // Member functions
    /// This function resizes the renderer and image
    void Resize();
    /// This functin renders the image per pixel
    void Render();
    
    /// This function returns the color value of each pixel
    /// - Parameters:
    ///   - x: x index of pixle
    ///   - y: y index of pixel
    glm::vec4 PerPixel(uint32_t x, uint32_t y);
    /// This funtion retrn the pixel color for hit or mis
    /// - Parameter ray: ray of camera
    /// - Parameter bounce: bounce of ray
    glm::vec3 RayColor(const Ray& ray, int32_t bounce);
    /// This function trace the rays on the hitable obkects
    /// - Parameters:
    ///   - ray: ray of camera
    ///   - payload: hit ppayload output
    bool TraceRay(const Ray& ray, HitPayload& payload);

    // member variables
    std::shared_ptr<Image> final_image_ = nullptr;
    uint32_t* image_data_ = nullptr;
    uint32_t viewport_width_ = 900, viewport_height_ = 600;
    EditorCamera editor_camera_ = EditorCamera(true);
    
    std::vector<Sphere> spheres;
  };
  
}
