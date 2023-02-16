//
//  runtime_item.hpp
//  kreator
//
//  Created by Ashish . on 16/02/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CommonRuntimeData {
  public:
    void AddRuntimeItemComponents(Entity* entity);
    void LivingEntityHitCheck(Entity* collided_entity, b2Contact* contact);
    void CheckAndDestroy(Entity* entity);
    
  protected:
    RigidBodyComponent* rigid_body_comp_;
    bool hit_player_ = false;
    bool destroy_ = false;
  };
  
  class CoinController : public ScriptableEntity {
  public:
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    
  private:
    glm::vec2 top_pos_;
    float speed_ = 8.0f;
  };
  
  class MushroomController : public ScriptableEntity, CommonRuntimeData {
  public:
    ~MushroomController() {
      
    }
    
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& normal) override;
    
  private:
    bool going_right_ = true;
    float max_speed_ = 8.0f;
    float free_fall_factor = 1.0f;
    glm::vec2 velocity_ = { 4.0f, 0.0f };
  };
  
  class FlowerController : public ScriptableEntity, CommonRuntimeData {
  public:
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& normal) override;
  };
  
  enum class Items {
    Coin, Mushroom, Flower
  };
  
  class RuntimeItem {
  public:
    static void Init();
    static void Shutdown();
    static void Create(Items item, EnttScene* scene, const glm::vec2& pos);
  };
  
}

