//
//  coin.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CoinController : public ScriptableEntity {
  public:
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    
  private:
    glm::vec2 top_pos_;
    float speed_ = 8.0f;
  };

  class MushroomController : public ScriptableEntity {
  public:
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& normal) override;

  private:
    RigidBodyComponent* rigid_body_comp_;
    glm::vec2 velocity_ = { 4.0f, 0.0f };
    float max_speed_ = 8.0f;
    bool going_right_ = true;
    bool hit_player_ = false;
    
    bool destroy = false;
  };

  enum class Items {
    Coin, Mushroom
  };
  
  class RuntimeItem {
  public:
    static void Init();
    static void Shutdown();
    static void Create(Items item, EnttScene* scene, const glm::vec2& pos);
  };
    
}

