//
//  common.h
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "settings.hpp"

namespace physics {
  
#define NOT_USED(x) ((void)(x))
  
#define LinearSlop      (0.005f * LengthUnitsPerMeter)
#define PolygonRadius    (2.0f * LinearSlop)

}
