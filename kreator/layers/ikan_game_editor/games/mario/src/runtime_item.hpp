//
//  runtime_item.hpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CommonRuntimeData {
  public:
    void AddRuntimeItemComponents(Entity* entity);
    void LivingEntityHitCheck(Entity* collided_entity, b2Contact* contact, Entity* curr_entity);
    void CheckAndDestroy(Entity* entity);
    
  protected:
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
    ~MushroomController() = default;
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& normal) override;
    void Copy(void* script) override;
    
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
  
  class ScoreController : public ScriptableEntity {
  public:
    ScoreController(int32_t score) : score_(score) {}
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void Copy(void* script) override;

  private:
    glm::vec2 top_pos_;
    float speed_ = 8.0f;
    int32_t score_ = 0;
  };
  
  enum class Items {
    Coin, Mushroom, Flower, Score
  };
  
  class RuntimeItem {
  public:
    static void Init();
    static void Shutdown();
    static void Create(Items item, EnttScene* scene, const glm::vec2& pos, int32_t score);
  };
  
}

