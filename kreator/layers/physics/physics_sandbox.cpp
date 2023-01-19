//
//  physics_sandbox.cpp
//  kreator
//
//  Created by Ashish . on 18/01/23.
//

#include "physics_sandbox.hpp"

#include <physics/physics_headers.h>

namespace physics {
  
  PhysicsLayer::PhysicsLayer() : Layer("Kreator") {
    IK_INFO("Physics", "Creating Physics Layer instance ... ");
  }
  
  PhysicsLayer::~PhysicsLayer() {
    IK_WARN("Physics", "Destroying Physics Layer instance !!! ");
  }
  
  void PhysicsLayer::Attach() {
    IK_INFO("Physics", "Attaching Physics Layer instance");
    
    Vec3 a(2, 3, 4), b(5, 6, 7), c(8, 9, 3);
    a.Log();
    
    Mat33 m(a, b, c);
    m.Log();
    
    auto rr = glm::radians(45.0f);
    Rot r(rr);
    r.Log();
    
    BlockAllocator block;
    block.Allocate(200);
    block.Allocate(600);
    
  }
  
  void PhysicsLayer::Detach() {
    IK_WARN("Physics", "Detaching Physics Layer instance ");
  }
  
  void PhysicsLayer::Update(Timestep ts) {

  }
  
  void PhysicsLayer::EventHandler(Event& event) {
  }
  
  void PhysicsLayer::RenderGui() {
  }
  
}
