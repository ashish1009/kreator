//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

#include "renderer_layer.hpp"

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec):
  ikan::Application(application_spec) {
    IK_INFO("Kreator Application", "Creating Kreator Application Instance ...");
    
    PushLayer(std::make_shared<kreator::RendererLayer>());
  }
  ~KreatorApp() {
    IK_WARN("Kreator Application", "Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
  
  // Platform
  application_spec.rendering_api = ikan::Renderer::Api::OpenGl;
  application_spec.os = ikan::OperatingSystem::Mac;
    
  return std::make_unique<KreatorApp>(application_spec);
}
