//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "layers/ray_tracing/src/renderer_layer.hpp"
#include "layers/ikan_game_editor/src/renderer_layer.hpp"

enum class LayerType : uint8_t {
  Editor,
  RayTracing,
  /* Add More with time */
};

bool new_mario = true;

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec, LayerType application_type):
  ikan::Application(application_spec) {
    IK_INFO("Kreator Application", "Creating Kreator Application Instance ...");
    
    switch (application_type) {
      case LayerType::Editor :
        break;
      case LayerType::RayTracing :
        PushLayer(std::make_shared<ray_tracing::RendererLayer>());
        break;
    };
  }
  ~KreatorApp() {
    IK_WARN("Kreator Application", "Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Set up the type of applicaiton we want to create
  LayerType application_type = LayerType::RayTracing;
  
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
  application_spec.rendering_api = ikan::Renderer::Api::OpenGl;
  
  application_spec.os = ikan::OperatingSystem::Mac;
  
  // Window Specification
  application_spec.window_specification.title = "Untitled Window";
  application_spec.window_specification.width = 1600;
  application_spec.window_specification.height = 900;
  application_spec.window_specification.v_sync = true;
  application_spec.window_specification.fullscreen = false;
  application_spec.window_specification.decorated = true;
  
  application_spec.resizable = true;
  application_spec.start_maximized = false;
  
  // Create the instance of applciaiton based on the type of suppored applucaiton
  switch (application_type) {
    case LayerType::Editor :
      application_spec.name = "Kreator";
      application_spec.window_specification.title = "Kreator";
      application_spec.client_asset_path = "../../../kreator/layers/ecs_editor/editor_assets/";
      application_spec.save_ini_file_path = "../../../kreator/layers/ecs_editor/ecs.ini";
      break;
    case LayerType::RayTracing :
      application_spec.name = "RayTracing";
      application_spec.window_specification.title = "RayTracing";
      application_spec.client_asset_path = "../../../kreator/layers/ray_tracing/assets/";
      application_spec.save_ini_file_path = "../../../kreator/layers/ray_tracing/ray_tracing.ini";
      break;
  };
  return std::make_unique<KreatorApp>(application_spec, application_type);
}
