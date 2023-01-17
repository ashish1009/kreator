//
//  core_entry_point.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

/// This Function should be defined in client side for creating client application derived from base ikan::Application
/// - Returns shared pointer of created application
extern std::unique_ptr<ikan::Application> CreateApplication();

/// This is the Entiry point defined at the core engine. This function
/// - Initializes the Engine (Spd Logger)
/// - Creates the application
/// - Run the Game loop
/// - Destroy the application
/// - Warning: If this file is included at the client side then there should not be any other entry point. else crearte New entry point
int main() {
  // Initialize the ikan Logger
  auto core_level   = ikan::Logger::Level::Trace;
  auto client_level = ikan::Logger::Level::Trace;
  
  ikan::Logger::Init(core_level, /* Core Log Level */
                     client_level, /* Client Log Level */
                     "../../../log", /* Log saving folder */
                     "ikan_log");     /* Log file name to be saved */
  
#ifdef IK_DEBUG_FEATURE
  auto spd_core_log_level = ikan::Logger::GetSpdLevelFromKreatorLevel(core_level);
  auto spd_client_log_level = ikan::Logger::GetSpdLevelFromKreatorLevel(client_level);
  
  IK_INFO("Core Entry Point", "Initialized the spd logger ");
  IK_INFO("Core Entry Point", "  Core   | {0}", ikan::Logger::GetLogLevelStringFromSpdLevel(spd_core_log_level));
  IK_INFO("Core Entry Point", "  Client | {0}", ikan::Logger::GetLogLevelStringFromSpdLevel(spd_client_log_level));
#endif
  
  auto app = CreateApplication();
  if (app) {
    app->Run();
  }

  return 0;
}
