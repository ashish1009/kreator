//
//  batch_2d_renderer.hpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#pragma once

#include "core/math/aabb.hpp"

// This file includes batch renderer that is resposible for
// - Initialize all Renderer buffer and store them for rendering 1 Batch in single
// draw call
// - User friendly API to render any 2D Shape (For now Quad and Cirlce)

namespace ikan {
  
  class Texture;
  class SubTexture;
  
  class BatchRenderer {
  public:
    /// This function initialzes the Batch renderer. Create All buffers needed to store Data (Both Renderer and CPU)
    static void Init(uint32_t max_quads, uint32_t max_cirlces, uint32_t max_lines);
    /// This functio dhutdown the batch renderer and destoy all the buffer reserved on initialize
    static void Shutdown();
    
    /// This function begins the Batch for 2D Rendere (to be called each frame)
    /// - Parameters:
    ///   - camera_view_projection_matrix: Camera View projection Matrix
    static void BeginBatch(const glm::mat4& camera_view_projection_matrix);
    /// This function Ends the current batch by rendering all the vertex
    static void EndBatch();
    
    /// This funcition initialize the quad renderer data
    /// - Parameter max_quads: max quad to be renderered in single batch
    static void InitQuadData(uint32_t max_quads = 50);
    /// This funcition initialize the circle renderer data
    /// - Parameter max_circles: max circle to be renderered in single batch
    static void InitCircleData(uint32_t max_circles = 50);
    /// This funcition initialize the lines renderer data
    /// - Parameter max_lines: max circle to be renderered in single batch
    static void InitLineData(uint32_t max_lines = 100);
    
    /// This functin returns the max quads in batch
    static uint32_t MaxQuads();
    /// This functin returns the max circle in batch
    static uint32_t MaxCircles();
    /// This functin returns the max lines in batch
    static uint32_t MaxLines();

    // ---------------------
    // Draw API
    // ---------------------
    /// This function draws Quad with color
    /// - Parameters:
    ///   - transform: Transformation matrix of Quad
    ///   - color: Color of Quad
    ///   - object_id: Pixel ID of Quad
    static void DrawQuad(const glm::mat4& transform,
                         const glm::vec4& color,
                         int32_t object_id = -1 );
    /// This function draws Quad with texture
    /// - Parameters:
    ///   - transform: Transformation matrix of Quad
    ///   - texture: Texture to be uploaded in Batch
    ///   - tint_color: Color of Quad
    ///   - tiling_factor: tiling factor of Texture (Scale by which texture to be Multiplied)
    ///   - object_id: Pixel ID of Quad
    static void DrawQuad(const glm::mat4& transform,
                         const std::shared_ptr<Texture>& texture,
                         const glm::vec4& tint_color = glm::vec4(1.0f),
                         float tiling_factor = 1.0f,
                         int32_t object_id = -1 );
    /// This function draws Quad with Subtexture sprite
    /// - Parameters:
    ///   - transform: Transformation matrix of Quad
    ///   - sub_texture: Subtexture component
    ///   - object_id: entity ID of Quad
    static void DrawQuad(const glm::mat4& transform,
                         const std::shared_ptr<SubTexture>& sub_texture,
                         const glm::vec4& tint_color = glm::vec4(1.0f),
                         int32_t object_id = -1);

    /// This function draws circle with color
    /// - Parameters:
    ///   - position: Center Circle
    ///   - radius: radius
    ///   - rotation: rotation
    ///   - color: Color of Circle
    ///   - thickness: thickneess of Circle
    ///   - fade: fade of Circle Edge
    ///   - object_id: Pixel ID of Circle
    static void DrawCircle(const glm::vec3& position,
                           const glm::vec3& radius,
                           const glm::vec3& rotation,
                           const glm::vec4& color,
                           float thickness = 1.0f,
                           float fade = (float)0.005,
                           int32_t object_id = -1);

    /// This function draws circle with color
    /// - Parameters:
    ///   - transform: Transformation matrix of Circle
    ///   - color: Color of Circle
    ///   - thickness: thickneess of Circle
    ///   - fade: fade of Circle Edge
    ///   - object_id: Pixel ID of Circle
    static void DrawCircle(const glm::mat4& transform,
                           const glm::vec4& color,
                           float thickness = 1.0f,
                           float fade = (float)0.005,
                           int32_t object_id = -1);
    /// This function draws circle with texture
    /// - Parameters:
    ///   - transform: Transformation matrix of Circle
    ///   - texture: Texture to be uploaded in Batch
    ///   - tiling_factor: tiling factor of Texture (Scale by which texture to be Multiplied)
    ///   - tint_color: Color of Quad
    ///   - thickness: thickneess of Circle
    ///   - fade: fade of Circle Edge
    ///   - object_id: Pixel ID of Circle
    static void DrawCircle(const glm::mat4& transform,
                           const std::shared_ptr<Texture>& texture,
                           const glm::vec4& tint_color = glm::vec4(1.0f),
                           float tiling_factor = 1.0f,
                           float thickness = 1.0f,
                           float fade = (float)0.005,
                           int32_t object_id = -1 );
    
    /// This function draws a line using two points in 3d Space
    /// - Parameters:
    ///   - p0: fisrt point
    ///   - p1: second point
    ///   - color: Color of line
    static void DrawLine(const glm::vec3& p0,
                         const glm::vec3& p1,
                         const glm::vec4& color);
    /// This function draws a Recttangle using position and size of rectangle
    /// - Parameters:
    ///   - p0: Top left Point
    ///   - p1: Bottom Right Point
    ///   - color: Color of line
    static void DrawRect(const glm::vec3& p0,
                         const glm::vec3& p1,
                         const glm::vec4& color);

    /// This function draws a Recttangle using position and size of rectangle
    /// - Parameters:
    ///   - position: position of rectangle
    ///   - size: size of rectang;e
    ///   - color: Color of line
    static void DrawRect(const glm::vec3& position,
                         const glm::vec2& size,
                         const glm::vec4& color);
    /// This function draws a Recttangle using position and size of rectangle
    /// - Parameters:
    ///   - aabb: aabb of rectangle
    ///   - color: Color of line
    static void DrawRect(const AABB& aabb,
                         const glm::vec4& color);
    /// This function draws a Recttangle using position and size of rectangle
    /// - Parameters:
    ///   - transform: transform
    ///   - color: Color of line
    static void DrawRect(const glm::mat4& transform,
                         const glm::vec4& color);
    
    MAKE_PURE_STATIC(BatchRenderer);
    
  private:
    // Member Methods
    /// This function flsh a single batch
    static void Flush();
    /// This function moves to next batch in single frame
    static void NextBatch();
    
    // ---------------------------------------------------
    // Internal Helper API for Rendering Quad and Circle
    // ---------------------------------------------------
    /// This function is the helper function to rendere the quad
    /// - Parameters:
    ///   - transform: transform matrix of quad
    ///   - texture: texture to be binded in quad
    ///   - texture_coords: texture coordinates
    ///   - tiling_factor: tiling factor of texture
    ///   - tint_color: color of quad
    ///   - object_id: object/pixel id
    static void DrawTextureQuad(const glm::mat4& transform,
                                const std::shared_ptr<Texture>& texture,
                                const glm::vec2* texture_coords,
                                float tiling_factor,
                                const glm::vec4& tint_color,
                                int32_t object_id);

    /// This function is the helper function to rendere the circle
    /// - Parameters:
    ///   - transform: transform matrix of circle
    ///   - texture: texture to be binded in circle
    ///   - tiling_factor: tiling factor of texture
    ///   - tint_color: color of circle
    ///   - thickness: thickness of circle
    ///   - fade: cirlce face
    ///   - object_id: object/pixel id
    static void DrawTextureCircle(const glm::mat4& transform,
                                  const std::shared_ptr<Texture>& texture,
                                  float tiling_factor,
                                  const glm::vec4& tint_color,
                                  float thickness,
                                  float fade,
                                  int32_t object_id);

  };
  
}
