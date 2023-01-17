//
//  logger.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

#include <spdlog/spdlog.h>
#include "log_utils.hpp"

namespace ikan {
  
  /// This class is the wrapper the wrapepr class for logging. This wrapper class is using submodule
  /// spd-log from "https://github.com/gabime/spdlog"
  class Logger {
  public:
    /// This enum hold the Log level of ikan engine
    enum class Level : uint8_t {
      Trace, Debug, Info, Warning, Error, Critical
    };
    /// This enum hold the Project module of log
    enum class Type : uint8_t {
      Core, Client
    };
    /// This stores the Log Module details
    ///  - Flag to enable the Module log
    ///  - Filter of log level for this module
    struct TagDetails {
      bool enabled = true;
      Level level_filter = Level::Trace;
    };
    
    /// This function initializes the spd logger. Create instance for both core and client. Sets the core
    /// and client log levels as 'core_level' and 'client_level'. Create a file at path 'log_file_path' to
    /// save the logs in a file. Also set the format for the logger
    /// - Current format is [%T:%e:%f] [%-8l] [%-4n] : %v
    /// - where :
    ///   - %T : Time stamp as hh:mm:ss
    ///   - %e : Time stamp in milliseconds
    ///   - %f : Time stamp in microseconds
    ///   - %l : Log lebel string (-8 measn width reserved for the same)
    ///   - %n : Logger Type (core or client) (-4 is width reserved for the same)
    ///
    /// - Parameter - Core log Level:
    /// - Parameter - Client Log Level:
    /// - Parameter - Path relative to executable binary, where our log file to be saved :
    /// - Parameter - Log file name:
    /// - Important: Add '\' in the end of 'log_file_path' and ''log_file_name' should be wiothout extension
    static void Init(Level core_level,
                     Level client_level,
                     const std::string& log_file_path,
                     const std::string& log_file_name);
    
    /// This function converts spd log level type to string for debug printing
    /// - Parameter - spd log level enum:
    static const char* GetLogLevelStringFromSpdLevel(spdlog::level::level_enum level);
    
    /// This function converts ikan log level to the spd log level
    /// - Parameter - ikan log level enum:
    static spdlog::level::level_enum GetSpdLevelFromKreatorLevel(Logger::Level level);
    
    /// This function returns the shared pointer of Core log instance
    static std::shared_ptr<spdlog::logger>& GetCoreLogger();
    
    /// This function returns the shared pointer of Client log instance
    static std::shared_ptr<spdlog::logger>& GetClientLogger();
    
    /// This functoin Disables the module log
    /// - Parameter - tag name to be disabled:
    static void DisableModule(LogModule tag);
    /// This functoin Enables module log
    /// - Parameter - tag name to be enabled:
    static void EnableModule(LogModule tag);

    template<typename... Args>
    /// This function stores the log with tag of module
    /// - Parameter -  type of log project:
    /// - Parameter -  level of log:
    /// - Parameter -  tag of module:
    /// - Parameter -  Log string with argument:
    static void PrintMessage(Type type, Level level, LogModule tag, Args&&... args);
    
    template<typename... Args>
    /// This function stores the log with tag of module
    /// - Parameter -  type of log project:
    /// - Parameter -  level of log:
    /// - Parameter -  tag of module:
    /// - Parameter -  Log string with argument:
    static void PrintMessage(Type type, Level level, std::string_view tag, Args&&... args);

    ~Logger() noexcept = default;
    
  private:
    MAKE_PURE_STATIC(Logger);
    static std::shared_ptr<spdlog::logger> core_logger_, client_logger_;
    inline static std::map<std::string, TagDetails> enabled_tags_;
    
    /// this functun return the tag stored in logger
    /// - Parameter - tag of log module:
    static bool HasTag(const std::string& tag);
    /// This function return the enabled tags
    static std::map<std::string, TagDetails>& GetEnabledTags();
    
    /// Get the detail of a module tag
    /// - Parameter - tag of log module:
    static const TagDetails& GetDetail(const std::string& tag);
    
    template<typename... Args>
    /// This function stores the log with tag of module
    /// - Parameter - type of log project:
    /// - Parameter - level of log:
    /// - Parameter - tag of module:
    /// - Parameter - Log string with argument:
    static void PrintMessageImpl(Type type, Level level, std::string tag, Args&&... args);

  };
  
} // namespace ikan

#ifdef IK_ENABLE_LOG

// Client log macros
#define IK_TRACE(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Trace, tag, __VA_ARGS__)
#define IK_DEBUG(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Debug, tag, __VA_ARGS__)
#define IK_INFO(tag, ...)     ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Info, tag, __VA_ARGS__)
#define IK_WARN(tag, ...)     ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Warning, tag, __VA_ARGS__)
#define IK_ERROR(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Error, tag, __VA_ARGS__)
#define IK_CRITICAL(tag, ...) ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Client, ::ikan::Logger::Level::Critical, tag, __VA_ARGS__)

#else

// Client log macros
#define IK_TRACE(...)
#define IK_DEBUG(...)
#define IK_INFO(...)
#define IK_WARN(...)
#define IK_ERROR(...)
#define IK_CRITICAL(...)

#endif

// Example for LOG API
// use {i} for printing any variable at ith position in arguament
// IK_INFO(" ... string ... {0}, {1} .... ", Arg0, Arg1 ...);

namespace ikan {
  
  template<typename... Args>
  void Logger::PrintMessage(Logger::Type type, Logger::Level level, LogModule tag, Args&&... args) {
    PrintMessageImpl(type, level, GetModuleName(tag), std::forward<Args>(args)...);
  }
  
  template<typename... Args>
  void Logger::PrintMessage(Logger::Type type, Logger::Level level, std::string_view tag, Args&&... args) {
    PrintMessageImpl(type, level, GetModuleName(tag), std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Logger::PrintMessageImpl(Logger::Type type, Logger::Level level, std::string tag, Args&&... args) {
    static const uint32_t MaxTagLegth = 20;
    
    const auto& detail = GetDetail(std::string(tag));
    if (detail.enabled && detail.level_filter <= level) {
      auto logger = (type == Type::Core) ? GetCoreLogger() : GetClientLogger();
      std::string logString = std::string("[{0}" + std::string(size_t(MaxTagLegth - tag.size()), ' ') + std::string("] | {1}"));
      switch (level) {
        case Level::Debug:
          logger->debug(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
        case Level::Trace:
          logger->trace(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
        case Level::Info:
          logger->info(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
        case Level::Warning:
          logger->warn(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
        case Level::Error:
          logger->error(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
        case Level::Critical:
          logger->critical(logString, tag, fmt::format(std::forward<Args>(args)...));
          break;
      }
    }
  }

}
