//
//  collision.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  
  /// The features that intersect to form the contact point
  /// This must be 4 bytes or less.
  struct ContactFeature {
    enum class Type : uint8_t {
      Vertex = 0,
      Face = 1
    };
    
    uint8_t indexA;    ///< Feature index on shapeA
    uint8_t indexB;    ///< Feature index on shapeB
    uint8_t typeA;    ///< The feature type on shapeA
    uint8_t typeB;    ///< The feature type on shapeB
  };
  
  /// Contact ids to facilitate warm starting.
  union ContactID {
    ContactFeature cf;
    uint32_t key;          ///< Used to quickly compare contact ids.
  };
  
  /// A manifold point is a contact point belonging to a contact
  /// manifold. It holds details related to the geometry and dynamics
  /// of the contact points.
  /// The local point usage depends on the manifold type:
  /// -e_circles: the local center of circleB
  /// -e_faceA: the local center of cirlceB or the clip point of polygonB
  /// -e_faceB: the clip point of polygonA
  /// This structure is stored across time steps, so we keep it small.
  /// Note: the impulses are used for internal caching and may not
  /// provide reliable contact forces, especially for high speed collisions.
  struct ManifoldPoint {
    Vec2 local_point;    ///< usage depends on manifold type
    float normal_impulse;  ///< the non-penetration impulse
    float tangent_impulse;  ///< the friction impulse
    ContactID id;      ///< uniquely identifies a contact point between two shapes
  };
  
  /// A manifold for two touching convex shapes.
  /// Box2D supports multiple types of contact:
  /// - clip point versus plane with radius
  /// - point versus point with radius (circles)
  /// The local point usage depends on the manifold type:
  /// -e_circles: the local center of circleA
  /// -e_faceA: the center of faceA
  /// -e_faceB: the center of faceB
  /// Similarly the local normal usage:
  /// -e_circles: not used
  /// -e_faceA: the normal on polygonA
  /// -e_faceB: the normal on polygonB
  /// We store contacts in this way so that position correction can
  /// account for movement, which is critical for continuous physics.
  /// All contact scenarios must be expressed in one of these types.
  /// This structure is stored across time steps, so we keep it small.
  struct Manifold {
    enum class Type : uint8_t {
      Circles,
      FaceA,
      FaceB
    };
    
    ManifoldPoint points[MaxManifoldPoints];  ///< the points of contact
    Vec2 local_normal;                ///< not use for Type::e_points
    Vec2 local_point;                ///< usage depends on manifold type
    Type type;
    int32_t point_count;                ///< the number of manifold points
  };

  /// This is used to compute the current state of a contact manifold.
  struct WorldManifold {
    /// Evaluate the manifold with supplied transforms. This assumes
    /// modest motion from the original state. This does not change the
    /// point count, impulses, etc. The radii must come from the shapes
    /// that generated the manifold.
    void Initialize(const Manifold* manifold,
                    const Transform& xfA, float radiusA,
                    const Transform& xfB, float radiusB);
    
    Vec2 normal;                ///< world vector pointing from A to B
    Vec2 points[MaxManifoldPoints];    ///< world contact point (point of intersection)
    float separations[MaxManifoldPoints];  ///< a negative value indicates overlap, in meters
  };
  
  /// Ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
  struct RayCastInput {
    Vec2 p1, p2;
    float maxFraction;
  };
  
  /// Ray-cast output data. The ray hits at p1 + fraction * (p2 - p1), where p1 and p2 come from RayCastInput.
  struct RayCastOutput {
    Vec2 normal;
    float fraction;
  };
  
  /// An axis aligned bounding box.
  struct AABB {
    /// Verify that the bounds are sorted.
    bool IsValid() const;
    
    /// Get the center of the AABB.
    Vec2 GetCenter() const {
      return 0.5f * (lowerBound + upperBound);
    }
    
    /// Get the extents of the AABB (half-widths).
    Vec2 GetExtents() const {
      return 0.5f * (upperBound - lowerBound);
    }
    
    /// Get the perimeter length
    float GetPerimeter() const {
      float wx = upperBound.x - lowerBound.x;
      float wy = upperBound.y - lowerBound.y;
      return 2.0f * (wx + wy);
    }
    
    /// Combine an AABB into this one.
    void Combine(const AABB& aabb) {
      lowerBound = Min(lowerBound, aabb.lowerBound);
      upperBound = Max(upperBound, aabb.upperBound);
    }
    
    /// Combine two AABBs into this one.
    void Combine(const AABB& aabb1, const AABB& aab) {
      lowerBound = Min(aabb1.lowerBound, aab.lowerBound);
      upperBound = Max(aabb1.upperBound, aab.upperBound);
    }
    
    /// Does this aabb contain the provided AABB.
    bool Contains(const AABB& aabb) const {
      bool result = true;
      result = result && lowerBound.x <= aabb.lowerBound.x;
      result = result && lowerBound.y <= aabb.lowerBound.y;
      result = result && aabb.upperBound.x <= upperBound.x;
      result = result && aabb.upperBound.y <= upperBound.y;
      return result;
    }
    
    bool RayCast(RayCastOutput* output, const RayCastInput& input) const;
    
    Vec2 lowerBound;  ///< the lower vertex
    Vec2 upperBound;  ///< the upper vertex
  };
  
  inline bool TestOverlap(const AABB& a, const AABB& b) {
    Vec2 d1, d2;
    d1 = b.lowerBound - a.upperBound;
    d2 = a.lowerBound - b.upperBound;
    
    if (d1.x > 0.0f || d1.y > 0.0f)
      return false;
    
    if (d2.x > 0.0f || d2.y > 0.0f)
      return false;
    
    return true;
  }

  
}
