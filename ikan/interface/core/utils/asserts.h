//
//  assert.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes API for Trigger Assert

// Internal API Marco used by main MACRO
#define IK_EXPAND_VARGS(x) x
#define ASSERT(condition) assert(condition);

#define IK_ASSERT_NO_MESSAGE(condition) { \
if(!(condition)) { \
IK_ERROR(LogModule::Assert, "Assertion Failed"); \
ASSERT(condition); \
} \
}

#define IK_ASSERT_MESSAGE(condition, ...) { \
if(!(condition)) { \
IK_ERROR(LogModule::Assert, "Assertion Failed: {0}", __VA_ARGS__); \
ASSERT(condition); \
} \
} \

#define IK_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro

#define IK_GET_ASSERT_MACRO(...) \
IK_EXPAND_VARGS( \
IK_ASSERT_RESOLVE( \
__VA_ARGS__, \
IK_ASSERT_MESSAGE, \
IK_ASSERT_NO_MESSAGE \
) \
)

// API Macro to trigger the Abort with and without any prints
#define IK_ASSERT(...) \
IK_EXPAND_VARGS(\
IK_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__)\
)

#define IK_ABORT(condition) IK_ASSERT_NO_MESSAGE(condition);
