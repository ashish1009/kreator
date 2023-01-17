//
//  common.h
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
#define NOT_USED(x) ((void)(x))
  
#define MaxManifoldPoints  2
#define LinearSlop      (0.005f * LengthUnitsPerMeter)
#define AngularSlop      (2.0f / 180.0f * PI)
#define PolygonRadius    (2.0f * LinearSlop)
#define AabbExtension    (0.1f * LengthUnitsPerMeter)
#define AabbMultiplier    4.0f  
#define MaxAngularCorrection    (8.0f / 180.0f * PI)
#define Baumgarte        0.2f
#define ToiBaumgarte        0.75f
#define MaxLinearCorrection    (0.2f * LengthUnitsPerMeter)
#define MaxTranslation      (2.0f * LengthUnitsPerMeter)
#define MaxTranslationSquared  (MaxTranslation * MaxTranslation)
  
#define MaxRotation        (0.5f * PI)
#define MaxRotationSquared    (MaxRotation * MaxRotation)

#define TimeToSleep        0.5f
  
  /// A body cannot sleep if its linear velocity is above this tolerance.
#define LinearSleepTolerance    (0.01f * LengthUnitsPerMeter)
  
  /// A body cannot sleep if its angular velocity is above this tolerance.
#define AngularSleepTolerance  (2.0f / 180.0f * PI)

#define MaxTOIContacts      32
#define MaxSubSteps      8

}
