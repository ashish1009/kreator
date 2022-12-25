//
//  core_application.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "core_application.hpp"

namespace ikan {
  
  Application::Application() {
    IK_CORE_INFO("Creating Core Application Instance ...");

  }
  
  Application::~Application() {
    IK_CORE_WARN("Destroying Core Application Instance ...");
  }
}
