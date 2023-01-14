//
//  collision.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "collision.hpp"
#include "distance.hpp"

namespace physics {
  
  bool TestOverlap(  const Shape* shapeA, int32_t indexA,
                     const Shape* shapeB, int32_t indexB,
                     const Transform& xfA, const Transform& xfB) {
    DistanceInput input;
    input.proxyA.Set(shapeA, indexA);
    input.proxyB.Set(shapeB, indexB);
    input.transformA = xfA;
    input.transformB = xfB;
    input.useRadii = true;
    
    SimplexCache cache;
    cache.count = 0;
    
    DistanceOutput output;
    
    Distance(&output, &cache, &input);
    
    return output.distance < 10.0f * FLT_EPSILON;
  }

}
