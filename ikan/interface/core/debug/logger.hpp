//
//  logger.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the wrapepr class for logging. This wrapper class is using
// submodule spd-log from "https://github.com/gabime/spdlog"

#include <spdlog/spdlog.h>

namespace ikan {
  
  /// This class is the wrapper for creating instance of spd logger for both core and client. This is
  /// also responsible for initializing the spd logger
  class Logger {
  public:
    /// ikan Log Level enum class
    enum class Level : uint8_t {
      Trace, Debug, Info, Warning, Error, Critical
    };
    
    /// This function initializes the spd logger. Create instance for both core and client. Sets the core
    /// and client log levels as 'core_level' and 'client_level'. Create a file at path 'log_file_path' to
    /// save the logs in a file. Also set the format for the logger Current format is [%T:%e:%f] [%-8l] [%-4n] : %v
    /// where :
    ///   - %T : Time stamp as hh:mm:ss
    ///   - %e : Time stamp in milliseconds
    ///   - %f : Time stamp in microseconds
    ///   - %l : Log lebel string (-8 measn width reserved for the same)
    ///   - %n : Logger Type (core or client) (-4 is width reserved for the same)
    /// - Parameters:
    ///   - core_level: Core log Level
    ///   - client_level: Client Log Level
    ///   - log_file_path: Path relative to executable binary, where our log file to be saved.
    ///                      NOTE: Add '\' in the end.
    ///   - log_file_name: Log file name. NOTE: Without any extention.
    static void Init(Level core_level,
                     Level client_level,
                     const std::string& log_file_path,
                     const std::string& log_file_name);
    
    /// This function converts spd log level type to string for printing
    /// - Parameter level: spd log level
    static const char* GetLogLevelStringFromSpdLevel(spdlog::level::level_enum level);
    
    /// This function converts ikan log level to the spd log level
    /// - Parameter level: ikan log level
    static spdlog::level::level_enum GetSpdLevelFromKreatorLevel(Logger::Level level);
    
    /// This function returns the shared pointer of Core log instance
    static std::shared_ptr<spdlog::logger>& GetCoreLogger();
    
    /// This function returns the shared pointer of Client log instance
    static std::shared_ptr<spdlog::logger>& GetClientLogger();
    
    ~Logger() noexcept = default;
    
  private:
    MAKE_PURE_STATIC(Logger);
    static std::shared_ptr<spdlog::logger> core_logger_, client_logger_;
  };
  
} // namespace ikan

#ifdef IK_ENABLE_LOG

// Client log macros
#define IK_TRACE(...) ::ikan::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define IK_DEBUG(...) ::ikan::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define IK_INFO(...) ::ikan::Logger::GetClientLogger()->info(__VA_ARGS__)
#define IK_WARN(...) ::ikan::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define IK_ERROR(...) ::ikan::Logger::GetClientLogger()->error(__VA_ARGS__)
#define IK_CRITICAL(...) ::ikan::Logger::GetClientLogger()->critical(__VA_ARGS__)

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
