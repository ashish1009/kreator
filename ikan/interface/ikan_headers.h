//
//  ikan_headers.h
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes any core headers that should be included in client side.
// Client should include just one file to compile Core engine library

// To remove documentation warning in entt::entity
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wformat-security"

// C++ Files
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <dispatch/dispatch.h>
#include <fstream>
#include <cxxabi.h>

// Library Files
// glm math library
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// Imgui files
#include <imgui.h>
#include <ImGuizmo.h>

// Common Files
#include <ikan_common.h>

// Core Files
#include <core/debug/logger.hpp>
#include <core/debug/profiler.hpp>

#include <core/utils/asserts.h>
#include <core/utils/asset_manager.hpp>
#include <core/utils/buffers.hpp>
#include <core/utils/string_utils.hpp>

#include <core/math/maths.hpp>
#include <core/math/uuid.hpp>

#include <core/events/mouse_event.hpp>
#include <core/events/application_event.hpp>
#include <core/events/key_event.hpp>

#include <core/core_application.hpp>
#include <core/layer.hpp>
#include <core/input.hpp>

// Renderer
#include <renderer/utils/renderer.hpp>
#include <renderer/utils/renderer_stats.hpp>
#include <renderer/utils/batch_2d_renderer.hpp>
#include <renderer/utils/text_renderer.hpp>
#include <renderer/utils/aabb_renderer.hpp>

#include <renderer/graphics/shader.hpp>
#include <renderer/graphics/renderer_buffer.hpp>
#include <renderer/graphics/pipeline.hpp>
#include <renderer/graphics/texture.hpp>
#include <renderer/graphics/framebuffer.hpp>

// Editor
#include <editor/property_grid.hpp>
#include <editor/editor_camera.hpp>
#include <editor/content_browser_panel.hpp>

// ECS
#include <ecs/viewport.hpp>
#include <ecs/entity.hpp>
#include <ecs/entt_scene.hpp>
#include <ecs/core_components.hpp>
#include <ecs/scriptable_entity.hpp>
#include <ecs/scene_serialiser.hpp>
#include <ecs/scene_panel_manager.hpp>

// Imgui API
#include <imgui/imgui_api.hpp>

// Ray Tracing
#include <ray_tracing/ray.hpp>
#include <ray_tracing/ray_material.hpp>
#include <ray_tracing/ray_scene.hpp>
#include <ray_tracing/ray_renderer.hpp>
#include <ray_tracing/hit_payload.hpp>
#include <ray_tracing/ray_sphere.hpp>
