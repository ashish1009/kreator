//
//  draw.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "physics_math.hpp"

namespace physics {
  
  /// Implement and register this class with a b2World to provide debug drawing of physics entities in your game.
  class Draw {
  public:
    enum {
      ShapeBit           = 0x0001,  ///< draw shapes
      JointBit           = 0x0002,  ///< draw joint connections
      AabbBit            = 0x0004,  ///< draw axis aligned bounding boxes
      PairBit            = 0x0008,  ///< draw broad-phase pairs
      CenterOfMassBit    = 0x0010   ///< draw center of mass frame
    };
    
    /// Constructor to draw the Physics World
    Draw();
    virtual ~Draw() = default;
    
    /// This function updates the drawing flags.
    /// - Parameter flags: flag value
    void SetFlags(uint32_t flags);
    /// This function appends flags to the current flags.
    /// - Parameter flags: Flag value
    void AppendFlags(uint32_t flags);
    /// This function Clear flags from the current flags.
    /// - Parameter flags: f;ag value
    void ClearFlags(uint32_t flags);

    /// This function Draws a closed polygon provided in CCW order.
    /// - Parameters:
    ///   - vertices: vertice of polygon
    ///   - vertex_count: vertex cont
    ///   - color: color
    virtual void DrawPolygon(const Vec2* vertices, int32_t vertex_count, const glm::vec4& color) = 0;
    /// This function Draws a solid closed polygon provided in CCW order.
    /// - Parameters:
    ///   - vertices: vertice of polygon
    ///   - vertex_count: vertex cont
    ///   - color: color
    virtual void DrawSolidPolygon(const Vec2* vertices, int32_t vertexCount, const glm::vec4& color) = 0;
    /// This function Draws a circle.
    /// - Parameters:
    ///   - center: center of circle
    ///   - radius: readuis of circle
    ///   - color: color
    virtual void DrawCircle(const Vec2& center, float radius, const glm::vec4& color) = 0;
    /// This function Draws a solid circle.
    /// - Parameters:
    ///   - center: center of circle
    ///   - radius: readuis of circle
    ///   - axis: axis
    ///   - color: color
    virtual void DrawSolidCircle(const Vec2& center, float radius, const Vec2& axis, const glm::vec4& color) = 0;
    /// This function Draws a line segment.
    /// - Parameters:
    ///   - p1: point 1
    ///   - p2: point 2
    ///   - color: color    `
    virtual void DrawSegment(const Vec2& p1, const Vec2& p2, const glm::vec4& color) = 0;
    /// This function Draws a transform. Choose your own length scale.
    /// - Parameter a: transform.
    /// - Parameter xf: xf
    virtual void DrawTransform(const Transform& xf) = 0;
    /// This function Draws a point.
    /// - Parameters:
    ///   - p: position
    ///   - size: size
    ///   - color: color
    virtual void DrawPoint(const Vec2& p, float size, const glm::vec4& color) = 0;

    /// This function returns the drawing flags.
    uint32_t GetFlags() const;

  protected:
    uint32_t m_drawFlags;
  };
  

  
}
