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

// Common Files
#include <ikan_common.h>

// Core Files
#include <core/debug/logger.hpp>
#include <core/debug/profiler.hpp>
#include <core/utils/asserts.h>
#include <core/utils/asset_manager.hpp>
#include <core/utils/buffers.hpp>
#include <core/core_application.hpp>
#include <core/layer.hpp>

// Renderer
#include <renderer/utils/renderer.hpp>
#include <renderer/utils/renderer_stats.hpp>
#include <renderer/graphics/shader.hpp>
#include <renderer/graphics/renderer_buffer.hpp>
#include <renderer/graphics/pipeline.hpp>

// Imgui API
#include <imgui/imgui_api.hpp>
