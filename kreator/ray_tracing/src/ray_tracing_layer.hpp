//
//  ray_tracing_layer.hpp
//  kreator
//
//  Created by Ashish . on 30/12/22.
//

#pragma once

#include "scene.hpp"
#include "math.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "trianglemesh.hpp"
#include "camera.hpp"

namespace ray_tracing {
  
  using namespace ikan;

  struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
  };
  
  struct Material {
    glm::vec3 albedo;
    float roughness = 1.0f;
    float metallic = 0.0f;
  };
  
  struct Sphere {
    glm::vec3 position;
    float radius = 0.5f;
    int material_index;
  };
  
  class RayScene {
  public:
    std::vector<Sphere> shperes;
    std::vector<Material> materials;
  };

  class RayTracingLayer : public Layer {
  public:
    struct Setting {
      bool accumulate = true;
    };

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
    struct HitPayload {
      float hit_distance;
      glm::vec3 world_normal;
      glm::vec3 world_position;
      
      int32_t object_idx;
    };
    
    void Resize();
    void Render();
    
    glm::vec3 CastRay(const Ray_& r, const Shape& scene);

    glm::vec4 PerPixel(uint32_t x, uint32_t y);
    
    Ray_ get_ray(float u, float v);
    
    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hit_distance, int32_t object_idx);
    HitPayload Miss(const Ray& ray);

    std::shared_ptr<Image> final_image_ = nullptr;
    uint32_t* image_data_ = nullptr;
    uint32_t viewport_width_ = 900, viewport_height_ = 600;
    EditorCamera editor_camera_;
    RayScene scene_;
    uint32_t frame_index_ = 1;
    Setting setting_;
    glm::vec4* accumulation_data_ = nullptr;
    Scene scene;
  };
  
}
