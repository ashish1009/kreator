//
//  core_entry_point.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the Entry point for the ikan engine. Include this file
// in client to load core entry point or create a new entrypoint.

extern std::unique_ptr<ikan::Application> CreateApplication();

int main(int argc, const char * argv[]) {
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
  
  IK_INFO("Initialized the spd logger ");
  IK_INFO("  Core   | {0}", ikan::Logger::GetLogLevelStringFromSpdLevel(spd_core_log_level));
  IK_INFO("  Client | {0}", ikan::Logger::GetLogLevelStringFromSpdLevel(spd_client_log_level));
#endif
  
  auto app = CreateApplication();

  return 0;
}
