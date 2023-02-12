//
//  brick_script.h
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {

  class BrickController : public ScriptableEntity {
  public:
    BrickController() = default;
    ~BrickController() = default;
    
    void Create(Entity entity) override {}
    void Update(Timestep ts) override {}
    void RenderGui() override {}
    void BeginCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void EndCollision(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PreSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
    void PostSolve(Entity* collided_entity, b2Contact* contact, glm::vec2 normal) override {}
  };
  
}
