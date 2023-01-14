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
#define PolygonRadius    (2.0f * LinearSlop)

}
