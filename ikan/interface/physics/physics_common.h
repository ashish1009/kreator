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
#define PHYSICS_DEBUG 1

#if PHYSICS_DEBUG
#define PHYSICS_LOG(...) IK_CORE_INFO(ikan::LogModule::Physics, __VA_ARGS__);
#else
#define PHYSICS_LOG(...)
#endif

// --------------------
// Assert
// --------------------
#define PHYSICS_ASSERT(x) IK_CORE_ASSERT(x);

// -----------------------------------------------
// Constants
// -----------------------------------------------
#define  Epsilon    FLT_EPSILON
#define  PI         3.14159265359f

/// This is used to fatten AABBs in the dynamic tree. This allows proxies
/// to move by a small amount without triggering a tree adjustment.
/// This is in meters.
#define AabbExtension    (0.1f * LengthUnitsPerMeter)

/// This is used to fatten AABBs in the dynamic tree. This is used to predict
/// the future position based on the current displacement.
/// This is a dimensionless multiplier.
#define AabbMultiplier    4.0f
