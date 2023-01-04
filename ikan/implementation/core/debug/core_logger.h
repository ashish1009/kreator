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
#define IK_CORE_TRACE(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Trace, tag, __VA_ARGS__)
#define IK_CORE_DEBUG(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Debug, tag, __VA_ARGS__)
#define IK_CORE_INFO(tag, ...)     ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Info, tag, __VA_ARGS__)
#define IK_CORE_WARN(tag, ...)     ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Warning, tag, __VA_ARGS__)
#define IK_CORE_ERROR(tag, ...)    ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Error, tag, __VA_ARGS__)
#define IK_CORE_CRITICAL(tag, ...) ::ikan::Logger::PrintMessage(::ikan::Logger::Type::Core, ::ikan::Logger::Level::Critical, tag, __VA_ARGS__)

#else

// Core log macros
#define IK_CORE_TRACE(...)
#define IK_CORE_DEBUG(...)
#define IK_CORE_INFO(...)
#define IK_CORE_WARN(...)
#define IK_CORE_ERROR(...)
#define IK_CORE_CRITICAL(...)

#endif
