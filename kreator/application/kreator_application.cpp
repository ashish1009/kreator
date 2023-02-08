//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "layers/ecs_editor/src/editor_layer.hpp"
#include "layers/ray_tracing/src/renderer_layer.hpp"
#include "layers/mario/src/mario_main.hpp"
#include "layers/mario/new_game/renderer_layer.hpp"
#include "layers/chess/src/chess_main.hpp"

enum class SupportedApplicationType : uint8_t {
  Editor,
  RayTracing,
  Chess,
  Mario,
  /* Add More with time */
};

bool new_mario = true;

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec,
             SupportedApplicationType application_type):
  ikan::Application(application_spec) {
    IK_INFO("Kreator Application", "Creating Kreator Application Instance ...");
    
    switch (application_type) {
      case SupportedApplicationType::Editor :
        PushLayer(std::make_shared<editor::EditorLayer>());
        break;
      case SupportedApplicationType::RayTracing :
        PushLayer(std::make_shared<ray_tracing::RendererLayer>());
        break;
      case SupportedApplicationType::Mario :
        if (new_mario)
          PushLayer(std::make_shared<ikan_game::RendererLayer>());
        else
          PushLayer(std::make_shared<mario::MarioLayer>());
        break;
      case SupportedApplicationType::Chess :
        PushLayer(std::make_shared<chess::ChessLayer>());
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
  SupportedApplicationType application_type = SupportedApplicationType::Mario;
  
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
    case SupportedApplicationType::Editor :
      application_spec.name = "Kreator";
      application_spec.window_specification.title = "Kreator";
      application_spec.client_asset_path = "../../../kreator/layers/ecs_editor/editor_assets/";
      application_spec.save_ini_file_path = "../../../kreator/layers/ecs_editor/ecs.ini";
      break;
    case SupportedApplicationType::RayTracing :
      application_spec.name = "RayTracing";
      application_spec.window_specification.title = "RayTracing";
      application_spec.client_asset_path = "../../../kreator/layers/ray_tracing/assets/";
      application_spec.save_ini_file_path = "../../../kreator/layers/ray_tracing/ray_tracing.ini";
      break;
    case SupportedApplicationType::Mario :
      application_spec.name = "Mario";
      application_spec.window_specification.title = "iKan Mario";
      application_spec.client_asset_path = "../../../kreator/layers/mario/assets/";
      if (new_mario)
        application_spec.save_ini_file_path = "../../../kreator/layers/mario/new_mario.ini";
      else
        application_spec.save_ini_file_path = "../../../kreator/layers/mario/mario.ini";
      break;
    case SupportedApplicationType::Chess :
      application_spec.name = "Chess";
      application_spec.window_specification.title = "Chess";
      application_spec.client_asset_path = "../../../kreator/layers/chess/assets/";
      application_spec.save_ini_file_path = "../../../kreator/layers/chess/chess.ini";
  };
  return std::make_unique<KreatorApp>(application_spec, application_type);
}
