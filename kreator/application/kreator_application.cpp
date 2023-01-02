//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "layers/sandbox/sandbox_layer.hpp"
#include "layers/ecs_editor/src/editor_layer.hpp"
#include "layers/ray_tracing/src/renderer_layer.hpp"
#include "layers/mario/src/mario_main.hpp"
#include "layers/chess/src/chess_main.hpp"

enum class SupportedApplicationType : uint8_t {
  Sandbox,
  Editor,
  RayTracing,
  Chess,
  Mario
  /* Add More with time */
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
      case SupportedApplicationType::Mario :
        PushLayer(std::make_shared<mario::MarioLayer>());
        break;
      case SupportedApplicationType::Chess :
        PushLayer(std::make_shared<chess::ChessLayer>());
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
  SupportedApplicationType application_type = SupportedApplicationType::Chess;
  
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
      application_spec.client_asset_path = "../../../kreator/layers/sandbox/assets/";
      break;
    case SupportedApplicationType::Editor :
      application_spec.name = "Kreator";
      application_spec.window_specification.title = "Kreator";
      application_spec.client_asset_path = "../../../kreator/layers/ecs_editor/assets/";
      break;
    case SupportedApplicationType::RayTracing :
      application_spec.name = "RayTracing";
      application_spec.window_specification.title = "RayTracing";
      application_spec.client_asset_path = "../../../kreator/layers/ray_tracing/assets/";
      break;
    case SupportedApplicationType::Mario :
      application_spec.name = "Mario";
      application_spec.window_specification.title = "Mario";
      application_spec.client_asset_path = "../../../kreator/layers/mario/assets/";
      break;
    case SupportedApplicationType::Chess :
      application_spec.name = "Chess";
      application_spec.window_specification.title = "Chess";
      application_spec.client_asset_path = "../../../kreator/layers/chess/assets/";
      break;

  };
  return std::make_unique<KreatorApp>(application_spec, application_type);
}
