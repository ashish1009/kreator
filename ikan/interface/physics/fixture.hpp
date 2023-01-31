//
//  fixture.hpp
//  ikan
//
//  Created by Ashish . on 31/01/23.
//

#pragma once

#include "shape/shape.hpp"

namespace physics {
  
  /// This holds contact filtering data.
  struct Filter {
    /// The collision category bits. Normally you would just set one bit.
    uint16_t category_bits = 0x0001;
    /// The collision mask bits. This states the categories that this shape would accept for collision.
    int16_t mask_bits =  0xFFFF;
    /// Collision groups allow a certain group of objects to never collide (negative) or always collide
    /// (positive). Zero means no collision group. Non-zero group filtering always wins against the mask bits.
    int16_t group_index = 0;
  };
  
  /// A fixture definition is used to create a fixture. This class defines an abstract fixture definition.
  /// You can reuse fixture definitions safely.
  struct FixtureDef {
    /// The shape, this must be set. The shape will be cloned, so you can create the shape on the stack.
    const Shape* shape = nullptr;
    
    /// Use this to store application specific fixture data.
    utils::FixtureUserData user_data;
    
    /// The friction coefficient, usually in the range [0,1].
    float friction = 0.2f;
    
    /// The restitution (elasticity) usually in the range [0,1].
    float restitution = 0.0f;
    
    /// Restitution velocity threshold, usually in m/s. Collisions above this
    /// speed have restitution applied (will bounce).
    float restitution_threshold = 1.0f * LengthUnitsPerMeter;
    
    /// The density, usually in kg/m^2.
    float density = 0.0f;
    
    /// A sensor shape collects contact information but never generates a collision
    /// response.
    bool is_sensor = false;
    
    /// Contact filtering data.
    Filter filter;
    
    /// This function logs the fixture data
    void Log();
  };
  
}
