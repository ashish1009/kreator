//
//  editor_layer.cpp
//  ecs_editor
//
//  Created by Ashish . on 28/12/22.
//

#include "editor_layer.hpp"

namespace editor {
  
  glm::vec3 block_p = {0, 0, 0};
  glm::vec3 base_p[8];

  glm::vec3 cam_p = {0, 0, 40};
  
  glm::vec3 block_r = {0, 0, 10};
  glm::vec3 base_r[2];

  glm::vec3 block_s = {1, 1, 1};
  glm::vec3 base_s[2];
  
  b2Body* p_body, *c_body, *b_body;

  EditorLayer::EditorLayer() : Layer("Kreator") {
    IK_INFO("Editor", "Creating Editor Layer instance ... ");
  }
  
  EditorLayer::~EditorLayer() {
    IK_WARN("Editor", "Destroying Editor Layer instance !!! ");
  }
  
  void EditorLayer::Attach() {
    IK_INFO("Editor", "Attaching Editor Layer instance");
    camera_.SetProjectionType(SceneCamera::ProjectionType::Perspective);
    
    physics_world_ = new b2World({0, -19.8f});
    
    base_p[0] = {0, -15, 0};
    base_p[1] = {15, 0, 0};
    base_p[2] = {0, 15, 0};
    base_p[3] = {-15, 0, 0};
    
    base_p[4] = {0, -12, 0};
    base_p[5] = {12, 0, 0};
    base_p[6] = {0, 12, 0};
    base_p[7] = {-12, 0, 0};

    base_s[0] = {31, 1, 1};
    base_s[1] = {1, 31, 1};
    
    base_r[0] = {0, 0, 0};
    base_r[1] = {0, 0, 10};
    
    // Block
    {
      b2BodyDef body_def;
      body_def.type = b2_dynamicBody;
      body_def.position.Set(block_p.x, block_p.y);
      body_def.angle = block_r.z;
            
      p_body = physics_world_->CreateBody(&body_def);
      p_body->SetFixedRotation(false);
      
      // Fixture
      {
        // Polygone Shape
        {
          glm::vec2 size = {0.5f, 0.5f};
          
          b2PolygonShape polygon_shape;
          polygon_shape.SetAsBox(size.x * block_s.x, size.y * block_s.y);
          
          b2FixtureDef fixture_def;
          fixture_def.shape = &polygon_shape;
          fixture_def.density = 1.0f;
          fixture_def.friction = 1.0f;
          fixture_def.restitution = 1.0f;
          fixture_def.restitutionThreshold = 0.5f;

          p_body->CreateFixture(&fixture_def);
        }
      }
    }

    // Base
    for (int i = 0; i < 8; i++) {
      b2BodyDef body_def;
      body_def.type = b2_staticBody;
      body_def.position.Set(base_p[i].x, base_p[i].y);
      body_def.angle = base_r[i / 4].z;

      b_body = physics_world_->CreateBody(&body_def);
      b_body->SetFixedRotation(false);

      // Fixture
      {
        // Polygone Shape
        {
          glm::vec2 size = {0.5f, 0.5f};
          
          b2PolygonShape polygon_shape;
          polygon_shape.SetAsBox(size.x * base_s[i % 2].x, size.y * base_s[i % 2].y);
          
          b2FixtureDef fixture_def;
          fixture_def.shape = &polygon_shape;
          fixture_def.density = 1.0f;
          fixture_def.friction = 0.5f;
          fixture_def.restitution = 0.0f;
          fixture_def.restitutionThreshold = 0.5f;
          
          b_body->CreateFixture(&fixture_def);

        }
      }
    }
  }
  
  void EditorLayer::Detach() {
    IK_WARN("Editor", "Detaching Editor Layer instance ");
  }
  
  void EditorLayer::Update(Timestep ts) {
    
    // Physics
    const int32_t velocity_iteration = 6;
    const int32_t position_iteration = 2;
    
    physics_world_->Step(ts, velocity_iteration, position_iteration);
    
    // Get Transform
    
    if (p_body != nullptr) {
      const auto& position = p_body->GetPosition();
      
      block_r.z = (p_body->GetAngle());
      block_p = {position.x, position.y, 0.0f};
    }
    
    Renderer::Clear({0.2, 0.2, 0.2, 1.0});
    
    BatchRenderer::BeginBatch(camera_.GetProjection() * glm::inverse(Math::GetTransformMatrix(cam_p, {0, 0, 0}, {1, 1, 1})));
    BatchRenderer::DrawQuad(Math::GetTransformMatrix(block_p, block_r, block_s), {1, 0, 0, 1});
    for (int i = 0; i < 8; i ++)
      BatchRenderer::DrawQuad(Math::GetTransformMatrix(base_p[i], base_r[i / 4], base_s[i % 2]), {1, 1, 1, 1});
    BatchRenderer::EndBatch();
  }
  
  void EditorLayer::EventHandler(Event& event) {
  }

  void EditorLayer::RenderGui() {
    ImGui::Begin("Camera");
    camera_.RenderGui();
    PropertyGrid::Float3("CameraPos", cam_p);
    ImGui::End();
    
    Renderer::Framerate();
  }
} 
