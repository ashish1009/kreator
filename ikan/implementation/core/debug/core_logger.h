//
//  core_logger.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// Includes API for Logging only for Core Engine not visible to client

#include "core/debug/logger.hpp"

#ifdef IK_ENABLE_LOG

// Core log macros
#define IK_CORE_TRACE(...) ::ikan::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define IK_CORE_DEBUG(...) ::ikan::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define IK_CORE_INFO(...) ::ikan::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define IK_CORE_WARN(...) ::ikan::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define IK_CORE_ERROR(...) ::ikan::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define IK_CORE_CRITICAL(...) ::ikan::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#else

// Core log macros
#define IK_CORE_TRACE(...)
#define IK_CORE_DEBUG(...)
#define IK_CORE_INFO(...)
#define IK_CORE_WARN(...)
#define IK_CORE_ERROR(...)
#define IK_CORE_CRITICAL(...)

#endif
