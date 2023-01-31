//
//  physics_common.h
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

// --------------------------------------------
// Debug
// --------------------------------------------
#define PHYSICS_LOG 1

// -----------------------------------------------
// Constants
// -----------------------------------------------
#define  Epsilon    FLT_EPSILON
#define  PI         3.14159265359f

/// This is used to fatten AABBs in the dynamic tree. This allows proxies
/// to move by a small amount without triggering a tree adjustment.
/// This is in meters.
#define AabbExtension     (0.1f * LengthUnitsPerMeter)

/// This is used to fatten AABBs in the dynamic tree. This is used to predict
/// the future position based on the current displacement.
/// This is a dimensionless multiplier.
#define AabbMultiplier    4.0f

/// A small length used as a collision and constraint tolerance. Usually it is
/// chosen to be numerically significant, but visually insignificant. In meters.
#define LinearSlop        (0.005f * LengthUnitsPerMeter)

/// The radius of the polygon/edge shape skin. This should not be modified. Making
/// this smaller means polygons will have an insufficient buffer for continuous collision.
/// Making it larger may create artifacts for vertex collision.
#define PolygonRadius     (2.0f * LinearSlop)

// --------------------
// IK Assert Wrapper
// --------------------
#define PHYSICS_ASSERT(x) IK_CORE_ASSERT(x);

// -------------------------------------------------
// IK Log Wrapper
// -------------------------------------------------
#if PHYSICS_LOG
#define PHYSICS_INFO(...) IK_CORE_INFO(ikan::LogModule::Physics, __VA_ARGS__);
#define PHYSICS_TRACE(...) IK_CORE_TRACE(ikan::LogModule::Physics, __VA_ARGS__);
#define PHYSICS_DEBUG(...) IK_CORE_DEBUG(ikan::LogModule::Physics, __VA_ARGS__);
#define PHYSICS_WARN(...) IK_CORE_WARN(ikan::LogModule::Physics, __VA_ARGS__);
#define PHYSICS_ERROR(...) IK_CORE_ERROR(ikan::LogModule::Physics, __VA_ARGS__);
#define PHYSICS_CRITICAL(...) IK_CORE_CRITICAL(ikan::LogModule::Physics, __VA_ARGS__);
#else
#define PHYSICS_INFO(...)
#define PHYSICS_TRACE(...)
#define PHYSICS_DEBUG(...)
#define PHYSICS_WARN(...)
#define PHYSICS_ERROR(...)
#define PHYSICS_CRITICAL(...)
#endif
