//
//  physics_headers.h
//  ikan
//
//  Created by Ashish . on 18/01/23.
//

#pragma once

// This file includes any core headers that should be included in client side.
// Client should include just one file to compile Core engine library

// To remove documentation warning in entt::entity
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wformat-security"

#include "physics/physics_common.h"
#include "physics/utils/settings.hpp"
#include "physics/utils/physics_math.hpp"
#include "physics/utils/block_allocator.hpp"
#include "physics/utils/stack_allocator.hpp"
#include "physics/utils/growable_stack.hpp"

#include "physics/shape/shape.hpp"

#include "physics/world.hpp"
