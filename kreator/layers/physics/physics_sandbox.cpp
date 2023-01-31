//
//  physics_sandbox.cpp
//  kreator
//
//  Created by Ashish . on 18/01/23.
//

#include "physics_sandbox.hpp"

#include <physics/physics_headers.h>

namespace physics {
  
  const glm::vec3 p1 = {0, 0, 0};
  const glm::vec3 p2 = {0, -5, 0};
  
  const glm::vec3 cp1 = {0, 0, 40};
  
  const glm::vec3 r1 = {0, 0, 0};
  const glm::vec3 r2 = {0, 0, 0};
  
  const glm::vec3 s1 = {1, 1, 1};
  const glm::vec3 s2 = {10, 1, 1};
  
  PhysicsLayer::PhysicsLayer() : Layer("Kreator") {
    IK_INFO("Physics", "Creating Physics Layer instance ... ");
  }
  
  PhysicsLayer::~PhysicsLayer() {
    IK_WARN("Physics", "Destroying Physics Layer instance !!! ");
  }
  
  void PhysicsLayer::Attach() {
    IK_INFO("Physics", "Attaching Physics Layer instance");
    
    camera_ = std::make_shared<SceneCamera>(SceneCamera::ProjectionType::Perspective);
    
    physics_world_ = new physics::World({0, -9.8f});
    
    // Block
    {
      physics::BodyDef body_def;
      body_def.type = BodyType::DynamicBody;
      body_def.position.Set(p1.x, p1.y);
      body_def.angle = r1.z;
      
      Body* body = physics_world_->CreateBody(&body_def);
    }
    
    // Base
    {
      physics::BodyDef body_def;
      body_def.type = BodyType::StaticBody;
      body_def.position.Set(p2.x, p2.y);
      body_def.angle = r2.z;
  
      Body* body = physics_world_->CreateBody(&body_def);
    }
    
  }
  
  void PhysicsLayer::Detach() {
    IK_WARN("Physics", "Detaching Physics Layer instance ");
    
    delete physics_world_;
    physics_world_ = nullptr;
  }
  
  void PhysicsLayer::Update(Timestep ts) {
    
    Renderer::Clear({0.2, 0.2, 0.2, 1.0});
    
    BatchRenderer::BeginBatch(camera_->GetProjection() * glm::inverse(Math::GetTransformMatrix(cp1, {0, 0, 0}, {1, 1, 1})));
    BatchRenderer::DrawQuad(Math::GetTransformMatrix(p1, r1, s1), {1, 0, 0, 1});
    BatchRenderer::DrawQuad(Math::GetTransformMatrix(p2, r2, s2), {1, 0, 1, 1});
    BatchRenderer::EndBatch();

  }
  
  void PhysicsLayer::EventHandler(Event& event) {
  }
  
  void PhysicsLayer::RenderGui() {
  }
  
}
