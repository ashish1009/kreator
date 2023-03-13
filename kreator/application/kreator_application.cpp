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
    IK_INFO("Kreator Application", "Creating Kreator Application Instance ...");
  }
  ~KreatorApp() {
    IK_WARN("Kreator Application", "Destroying Kreator Application Instance !!!");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  // Set up all the applicaiton specification
  ikan::Application::Specification application_spec;
  return std::make_unique<KreatorApp>(application_spec);
}
