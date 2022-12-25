//
//  ikan_common.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file Includes all common APIs, Preprocessors, enums or structures that
// will be need in both Core and Client


namespace ikan {
 
  /// Supported Operating Sytem by iKan Engine
  enum class OperatingSystem : uint8_t {
    None = 0, Mac = 1 /* Add More supported OS */
  };
  
}

/// This MACRO Deletes the Constructors (Default, Copy and Move ) and Operator = (Copy and Move)
/// for any class to make pure static class
#define MAKE_PURE_STATIC(x) \
x() = delete; \
x(const x&) = delete; \
x(x&&) = delete; \
x& operator =(const x&) = delete; \
x& operator =(x&&) = delete; \

/// This MACRO deletes the Constructors (Copy and Move ) and Operator = (Copy and Move) for
/// any class x to make a class singleton. NOTE: default constructor still need to be private
#define DELETE_COPY_MOVE_CONSTRUCTORS(x) \
x(const x&) = delete; \
x(x&&) = delete; \
x& operator=(const x&) = delete; \
x& operator =(x&&) = delete; \

/// This MACRO Defines the Constructors (Copy and Move ) and Operator = (Copy and Move) for any class x
#define DEFINE_COPY_MOVE_CONSTRUCTORS(x) \
x(const x&); \
x(x&&); \
x& operator=(const x&); \
x& operator =(x&&); \
