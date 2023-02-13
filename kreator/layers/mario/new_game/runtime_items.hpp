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
    void BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& normal) override;

  private:
    void ChangeDirection(const glm::vec2& contact_normal);
    
    const float fixed_vel = 8.0f;
    RigidBodyComponent* rigid_body_comp_;
    glm::vec2 velocity_;
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

