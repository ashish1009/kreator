//
//  logger.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace ikan {
  
  // Static member initialisation
  std::shared_ptr<spdlog::logger> Logger::core_logger_ = nullptr;
  std::shared_ptr<spdlog::logger> Logger::client_logger_ = nullptr;
  
  // Static globals
  static const std::string kLogFileExtension = ".ikan_log"; // Log file extention.
  
  void Logger::Init(Level core_level,
                    Level client_level,
                    const std::string& log_file_path,
                    const std::string& log_file_name) {
    std::cout << "Initialising SPD Logger ..." << std::endl;
    
    // Sink pointers to store logs at different places
    // - Terminal
    // - File
    std::vector<spdlog::sink_ptr> log_sinks;
    typedef spdlog::sinks::stdout_color_sink_mt terminal_log_sink;
    typedef spdlog::sinks::basic_file_sink_mt file_log_sink;
    
    // Create sink pointer to store logs in terminal. Here store means just
    // printing (temporary)
    log_sinks.emplace_back(std::make_shared<terminal_log_sink>());
    
    // Create sink pointer to store logs in a file
    std::string full_path_for_logs = log_file_path + "/" + log_file_name + kLogFileExtension;
    log_sinks.emplace_back(std::make_shared<file_log_sink>(
                                                           full_path_for_logs.c_str(),
                                                           true
                                                           ));
    
    // Set up the logging patterns as below
    // Current format is [%T:%e:%f] [%-8l] [%-4n] : %v
    // where :
    //   - %T : Time stamp as hh:mm:ss
    //   - %e : Time stamp in milliseconds
    //   - %f : Time stamp in microseconds
    //   - %l : Log lebel string (-8 measn width reserved for the same)
    //   - %n : Logger Type (core or client) (-4 is width reserved for the same)
    log_sinks[0]->set_pattern("[%T:%e | %-7l | %-4n] %v ");
    log_sinks[1]->set_pattern("[%T:%e | %-7l | %-4n] %v ");
    
    
    // Create the Core Logger
    core_logger_ = std::make_shared<spdlog::logger>(
                                                    "IKAN ",
                                                    begin(log_sinks),
                                                    end(log_sinks)
                                                    );
    spdlog::register_logger(core_logger_);
    
    auto spd_core_log_level = GetSpdLevelFromKreatorLevel(core_level);
    core_logger_->set_level(spd_core_log_level);
    core_logger_->flush_on(spd_core_log_level);
    
    // Create the Client Logger
    client_logger_ = std::make_shared<spdlog::logger>(
                                               "APP  ",
                                               begin(log_sinks),
                                               end(log_sinks)
                                               );
    spdlog::register_logger(client_logger_);
    
    auto spd_client_log_level = GetSpdLevelFromKreatorLevel(client_level);
    client_logger_->set_level(spd_client_log_level);
    client_logger_->flush_on(spd_client_log_level);
    
    // Debug prints
    std::cout << "    Saving Logs at path '" << full_path_for_logs.c_str() << "'" << std::endl;
    std::cout << "    Intiializing SPD Logger for Core with log level : '" << GetLogLevelStringFromSpdLevel(spd_core_log_level) << "'" << std::endl;
    std::cout << "    Intiializing SPD Logger for Client with log level : '" << GetLogLevelStringFromSpdLevel(spd_client_log_level) << "'" << std::endl;
    std::cout << " ------------------------------------------------------------- " << std::endl;
  }
  
  const char* Logger::GetLogLevelStringFromSpdLevel(spdlog::level::level_enum level) {
    switch (level) {
      case spdlog::level::level_enum::trace    : return "Trace";
      case spdlog::level::level_enum::debug    : return "Debug";
      case spdlog::level::level_enum::info     : return "Info";
      case spdlog::level::level_enum::warn     : return "Warning";
      case spdlog::level::level_enum::err      : return "Error";
      case spdlog::level::level_enum::critical : return "Critical";
      case spdlog::level::level_enum::off      : return "Off";
      case spdlog::level::level_enum::n_levels : return "No Level";
      default: break;
    }
    return "No Level";
  }
  
  spdlog::level::level_enum Logger::GetSpdLevelFromKreatorLevel(Logger::Level level) {
    switch (level) {
      case Logger::Level::Trace:    return spdlog::level::level_enum::trace;
      case Logger::Level::Debug:    return spdlog::level::level_enum::debug;
      case Logger::Level::Info:     return spdlog::level::level_enum::info;
      case Logger::Level::Warning:  return spdlog::level::level_enum::warn;
      case Logger::Level::Error:    return spdlog::level::level_enum::err;
      case Logger::Level::Critical: return spdlog::level::level_enum::critical;
      default:
        IK_CORE_ASSERT(false, "Invalid level");
    }
  }
  
  std::shared_ptr<spdlog::logger>& Logger::GetCoreLogger() {
    return core_logger_;
  }
  std::shared_ptr<spdlog::logger>& Logger::GetClientLogger() {
    return client_logger_;
  }
  
  bool Logger::HasTag(const std::string& tag) {
    return enabled_tags_.find(tag) != enabled_tags_.end();
  }
  std::map<std::string, Logger::TagDetails>& Logger::GetEnabledTags() {
    return enabled_tags_;
  }

  const Logger::TagDetails& Logger::GetDetail(const std::string& tag) {
    if (HasTag(tag))
      return enabled_tags_.at(tag);
    else
      return enabled_tags_[std::string(tag)];
  }
  void Logger::DisableModule(LogModule tag) {
    std::string tag_string = GetModuleName(tag);
    if (HasTag(tag_string))
      enabled_tags_.at(tag_string).enabled = false;
  }
  void Logger::EnableModule(LogModule tag) {
    std::string tag_string = GetModuleName(tag);
    if (HasTag(tag_string))
      enabled_tags_.at(tag_string).enabled = true;
  }
  
}
