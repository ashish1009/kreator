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
    
    physics_world_ = new physics::World({0, -9.8f});
  }
  
  void PhysicsLayer::Detach() {
    IK_WARN("Physics", "Detaching Physics Layer instance ");
    
    delete physics_world_;
    physics_world_ = nullptr;
  }
  
  void PhysicsLayer::Update(Timestep ts) {

  }
  
  void PhysicsLayer::EventHandler(Event& event) {
  }
  
  void PhysicsLayer::RenderGui() {
  }
  
}
