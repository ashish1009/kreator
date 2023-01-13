//
//  world_callbacks.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

namespace physics {
  
  class Fixture;
  class Joint;

  /// Joints and fixtures are destroyed when their associated body is destroyed. Implement this
  /// listener so that you may nullify references to these joints and shapes.
  class DestructionListener {
  public:
    virtual ~DestructionListener() {}
    
    /// This function Called when any joint is about to be destroyed due to the destruction of one of its attached bodies.
    virtual void SayGoodbye(Joint* joint) = 0;
    
    /// This function Called when any fixture is about to be destroyed due to the destruction of its parent body.
    virtual void SayGoodbye(Fixture* fixture) = 0;
  };
  
}
