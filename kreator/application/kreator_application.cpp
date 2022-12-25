//
//  kreator_application.cpp
//  kreator
//
//  Created by Ashish . on 25/12/22.
//

class KreatorApp : public ikan::Application {
public:
  KreatorApp() {
    IK_INFO("Creating Kreator Application Instance ...");
  }
  ~KreatorApp() {
    IK_INFO("Destroying Kreator Application Instance ...");
  }
};

/// This funtion implementatis the API for creating instance of Core::Application
std::unique_ptr<ikan::Application> CreateApplication() {
  return std::make_unique<KreatorApp>();
}
