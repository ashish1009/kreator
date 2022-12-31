//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "sandbox/sandbox_layer.hpp"
#include "ecs_editor/src/editor_layer.hpp"
#include "ray_tracing/src/ray_tracing_layer.hpp"
#include "ray_tracing/src/renderer_layer.hpp"

enum class SupportedApplicationType : uint8_t {
  Sandbox, Editor, RayTracing /* Add More with time */
};

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec,
             SupportedApplicationType application_type):
  ikan::Application(application_spec) {
    IK_INFO("Creating Kreator Application Instance ...");
    
    switch (application_type) {
      case SupportedApplicationType::Sandbox :
        PushLayer(std::make_shared<sandbox::SandboxLayer>());
        break;
      case SupportedApplicationType::Editor :
        PushLayer(std::make_shared<editor::EditorLayer>());
        break;
      case SupportedApplicationType::RayTracing :
        PushLayer(std::make_shared<ray_tracing::RendererLayer>());
        break;
    };
  }
  ~KreatorApp() {
    IK_WARN("Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Set up the type of applicaiton we want to create
  SupportedApplicationType application_type = SupportedApplicationType::RayTracing;
  
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
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
  switch (application_type) {
    case SupportedApplicationType::Sandbox :
      application_spec.name = "Sandbox";
      application_spec.window_specification.title = "Sandbox";
      application_spec.window_specification.width = 1200;
      application_spec.window_specification.height = 800;
      application_spec.client_asset_path = "../../../kreator/sandbox/assets/";
      break;
    case SupportedApplicationType::Editor :
      application_spec.name = "Kreator";
      application_spec.window_specification.title = "Kreator";
      application_spec.window_specification.width = 1600;
      application_spec.window_specification.height = 900;
      application_spec.client_asset_path = "../../../kreator/ecs_editor/assets/";
      break;
    case SupportedApplicationType::RayTracing :
      application_spec.name = "RayTracing";
      application_spec.window_specification.title = "RayTracing";
      application_spec.window_specification.width = 1600;
      application_spec.window_specification.height = 900;
      application_spec.client_asset_path = "../../../kreator/RayTracing/assets/";
      break;

  };
  return std::make_unique<KreatorApp>(application_spec, application_type);
}
