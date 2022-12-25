//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

class KreatorApp : public ikan::Application {
public:
  KreatorApp(const ikan::Application::Specification& application_spec):
  ikan::Application(application_spec) {
    IK_INFO("Creating Kreator Application Instance ...");
  }
  ~KreatorApp() {
    IK_WARN("Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  ikan::Application::Specification application_spec;
  application_spec.client_asset_path = "../../../kreator/sandbox/assets/";
  application_spec.rendering_api = ikan::Renderer::Api::OpenGl;
  
  return std::make_unique<KreatorApp>(application_spec);
}
