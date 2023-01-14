//
//  collision.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "collision.hpp"
#include "distance.hpp"

namespace physics {
  
  // Sutherland-Hodgman clipping.
  int32_t ClipSegmentToLine(ClipVertex vOut[2], const ClipVertex vIn[2],
                            const Vec2& normal, float offset, int32_t vertexIndexA) {
    // Start with no output points
    int32_t count = 0;
    
    // Calculate the distance of end points to the line
    float distance0 = Dot(normal, vIn[0].v) - offset;
    float distance1 = Dot(normal, vIn[1].v) - offset;
    
    // If the points are behind the plane
    if (distance0 <= 0.0f) vOut[count++] = vIn[0];
    if (distance1 <= 0.0f) vOut[count++] = vIn[1];
    
    // If the points are on different sides of the plane
    if (distance0 * distance1 < 0.0f) {
      // Find intersection point of edge and plane
      float interp = distance0 / (distance0 - distance1);
      vOut[count].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);
      
      // VertexA is hitting edgeB.
      vOut[count].id.cf.indexA = static_cast<uint8_t>(vertexIndexA);
      vOut[count].id.cf.indexB = vIn[0].id.cf.indexB;
      vOut[count].id.cf.typeA = (uint8_t)ContactFeature::Type::Vertex;
      vOut[count].id.cf.typeB = (uint8_t)ContactFeature::Type::Face;
      ++count;
      
      IK_ASSERT(count == 2);
    }
    
    return count;
  }
  
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
