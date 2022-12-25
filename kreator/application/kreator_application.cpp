//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "sandbox/sandbox_layer.hpp"

enum class SupportedApplicationType : uint8_t {
  Sandbox /* Add More with time */
};

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec,
             std::vector<std::shared_ptr<ikan::Layer>> layers):
  ikan::Application(application_spec) {
    IK_INFO("Creating Kreator Application Instance ...");
    
    for (const auto& layer: layers) {
      if (layer)
        PushLayer(layer);
    }
  }
  ~KreatorApp() {
    IK_WARN("Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Set up the type of applicaiton we want to create
  SupportedApplicationType application_type = SupportedApplicationType::Sandbox;
  
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
  application_spec.client_asset_path = "../../../kreator/sandbox/assets/";
  application_spec.rendering_api = ikan::Renderer::Api::OpenGl;
  
  application_spec.os = ikan::OperatingSystem::Mac;
  
  // Window Specification
  application_spec.window_specification.title = "Untitled Window";
  application_spec.window_specification.width = 1200;
  application_spec.window_specification.height = 800;
  application_spec.window_specification.v_sync = true;
  application_spec.window_specification.fullscreen = false;
  application_spec.window_specification.decorated = true;
  
  application_spec.resizable = true;
  application_spec.start_maximized = false;
  
  // Create the instance of applciaiton based on the type of suppored applucaiton
  std::vector<std::shared_ptr<ikan::Layer>> layers;
  switch (application_type) {
    case SupportedApplicationType::Sandbox :
      application_spec.name = "Sandbox";
      application_spec.window_specification.title = "Sandbox";
      layers.emplace_back(std::make_shared<sandbox::SandboxLayer>());
      break;
  };
  return std::make_unique<KreatorApp>(application_spec, layers);
}
