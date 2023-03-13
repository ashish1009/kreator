//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "renderer_layer.hpp"

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec, kreator::GameType game_type):
  ikan::Application(application_spec) {
    IK_INFO("Kreator Application", "Creating Kreator Application Instance ...");
    
    PushLayer(std::make_shared<kreator::RendererLayer>(game_type));
  }
  ~KreatorApp() {
    IK_WARN("Kreator Application", "Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Type of game to be run on application
  kreator::GameType game_type = kreator::GameType::None;
  
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
  
  // Platform
  application_spec.rendering_api = ikan::Renderer::Api::OpenGl;
  application_spec.os = ikan::OperatingSystem::Mac;
  
  application_spec.name = "Kreator Application";
  application_spec.client_asset_path = "../../../kreator/assets/";
  application_spec.save_ini_file_path = "../../../kreator/assets/ini/kreator.ini";

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
  switch (game_type) {
    case kreator::GameType::Mario :
      application_spec.name = "ikan Mario";
      application_spec.window_specification.title = "ikan Mario";
      application_spec.client_asset_path = "../../../kreator/mario/assets/";
      application_spec.save_ini_file_path = "../../../kreator/mario/assets/ini/ikan_mario.ini";
      break;
      
    default:
      break;
  };
  return std::make_unique<KreatorApp>(application_spec, game_type);
}
