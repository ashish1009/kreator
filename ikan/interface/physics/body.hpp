//
//  body.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  /// The body type.  static: zero mass, zero velocity, may be manually moved
  /// kinematic: zero mass, non-zero velocity set by user, moved by solver
  /// dynamic: positive mass, non-zero velocity determined by forces, moved by solver
  enum BodyType { StaticBody = 0, KinematicBody, DynamicBody };
  
  /// A body definition holds all the data needed to construct a rigid body.
  /// You can safely re-use body definitions. Shapes are added to a body after construction.
  struct BodyDef {
    
  };
  
  /// A rigid body. These are created via b2World::CreateBody.
  class Body {
    
  };
  
}
