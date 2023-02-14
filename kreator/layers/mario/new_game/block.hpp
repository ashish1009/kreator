//
//  brick_script.h
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

#include "player.hpp"

namespace mario {
  
  class BlockController : public ScriptableEntity {
  public:
    enum class Type {
      Empty, Coin, PowerUp, Star 
    };
    
    BlockController(Type type, uint32_t count);
    ~BlockController() = default;
    
    void Create(Entity entity) override;
    void Update(Timestep ts) override;
    void BeginCollision(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) override;
    
    void SetInactive() {
      auto& qc = entity_.GetComponent<QuadComponent>();
      const auto& tex = qc.texture_comp.component;
      qc.texture_comp.sprite = SubTexture::CreateFromCoords(tex, {27.0f, 27.0f});
      
      if (entity_.HasComponent<AnimationComponent>())
        entity_.RemoveComponent<AnimationComponent>();
      
      active_ = false;
    }

  private:
    void PlayerHit(PlayerController* pc);
    
    bool going_up_ = true;
    bool animation_ = false;
    glm::vec2 start_pos_;
    glm::vec2 end_pos_;
    
    bool active_ = true;
    float speed_ = 3.0f;
    
    Type type_ = Type::Empty;
    uint32_t count_ = 0;
  };
  
  class BlockScriptManager {
  public:
    static void Init();
    static void Shutdown();
    
    static ScriptLoaderFn GetLoaderFn(const std::string& tag);
    static BlockController::Type GetType(const std::string& tag);
    static uint32_t GetCount(const std::string& tag);
  };
  using BSM = BlockScriptManager;
  
  inline bool IsBlock(const std::string& tag) {
    return tag == "Brick" or
    tag == "Coin" or tag == "MultiCoin" or
    tag == "PowerUp";
  }

}