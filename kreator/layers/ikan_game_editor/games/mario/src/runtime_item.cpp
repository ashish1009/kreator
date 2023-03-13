//
//  runtime_item.cpp
//  kreator
//
//  Created by Ashish . on 11/03/23.
//

#include "runtime_item.hpp"
#include "sprite_manager.hpp"
#include "player.hpp"
#include "runtime_item.hpp"

namespace mario {
  
  void CommonRuntimeData::AddRuntimeItemComponents(Entity* entity) {
    RigidBodyComponent* rb;
    if (entity->HasComponent<RigidBodyComponent>()) {
      rb = &(entity->GetComponent<RigidBodyComponent>());
    }
    else {
      rb = &(entity->AddComponent<RigidBodyComponent>());
    }
    
    rb->type = b2_dynamicBody;
    rb->angular_velocity = 0.0f;
    rb->fixed_rotation = true;
    rb->SetGravityScale(0.0f);
    
    if (entity->HasComponent<CircleColliiderComponent>()) {
      auto& ccc = entity->GetComponent<CircleColliiderComponent>();
      ccc.runtime_fixture = new Entity((entt::entity)(*entity), entity->GetScene());
      ccc.friction = 0.0f;
    }
    else {
      auto& ccc = entity->AddComponent<CircleColliiderComponent>();
      ccc.runtime_fixture = new Entity((entt::entity)(*entity), entity->GetScene());
      ccc.friction = 0.0f;
    }
    entity->GetScene()->AddBodyToPhysicsWorld(*entity, *rb);
  }
  
  void CommonRuntimeData::LivingEntityHitCheck(Entity* collided_entity, b2Contact* contact, Entity* curr_entity) {
    if (PlayerController* pc = PlayerController::Get();
        collided_entity->GetScene() and
        collided_entity->HasComponent<NativeScriptComponent>() and
        collided_entity->GetComponent<NativeScriptComponent>().script.get() == pc) {
      contact->SetEnabled(false);
      if (!hit_player_) {
        pc->SetPowerup();
        hit_player_ = true;
        destroy_ = true;
        
        auto& tc = curr_entity->GetComponent<TransformComponent>();
        RuntimeItem::Create(Items::Score, curr_entity->GetScene(), {tc.Translation().x - 0.5, tc.Translation().y + 1}, BonusScore);
      }
      return;
    }
  }
  
  void CommonRuntimeData::CheckAndDestroy(Entity* entity) {
    if (destroy_) {
      entity->GetScene()->DestroyEntity(*entity);
      PlayerController::Get()->AddScore(BonusScore);
      return;
    }
  }
  
  void CoinController::Create(Entity entity) {
    entity_ = entity;
    const auto& tc = entity_.GetComponent<TransformComponent>().Translation();
    top_pos_ = {tc.x, tc.y + 5.0f};
  }
  
  void CoinController::Update(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    if (tc.Translation().y < top_pos_.y) {
      tc.AddTranslation_Y(ts * speed_);
      tc.AddScale_X(-fmod((2.5f * ts), -1.0f));
    }
    else {
      entity_.GetScene()->DestroyEntity(entity_);
      PlayerController::Get()->AddCoin();
    }
  }
  
  void MushroomController::Create(Entity entity) {
    entity_ = entity;
    AddRuntimeItemComponents(&entity_);
    velocity_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * free_fall_factor;
  }
  
  void MushroomController::Update(Timestep ts) {
    auto& rb = entity_.GetComponent<RigidBodyComponent>();
    
    if (going_right_ and std::abs(rb.velocity.x) < max_speed_) {
      rb.SetVelocity(velocity_);
    }
    else if (!going_right_ and std::abs(rb.velocity.x) < max_speed_) {
      rb.SetVelocity({-velocity_.x, velocity_.y});
    }
    
    CheckAndDestroy(&entity_);
  }
  
  void MushroomController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    LivingEntityHitCheck(collided_entity, contact, &entity_);
    if (std::abs(contact_normal.y) < 0.1f) {
      going_right_ = contact_normal.x < 0.0f;
    }
  }
  
  void MushroomController::Copy(void* script) {
    if (!script)
      return;
    
    MushroomController* mushroom_script = reinterpret_cast<MushroomController*>(script);
    IK_ASSERT(mushroom_script);
    
    going_right_ = mushroom_script->going_right_;
    max_speed_ = mushroom_script->max_speed_;
    free_fall_factor = mushroom_script->free_fall_factor;
    velocity_ = mushroom_script->velocity_;
  }
  
  void FlowerController::Create(Entity entity) {
    entity_ = entity;
    AddRuntimeItemComponents(&entity_);
    
    if (!entity.HasComponent<AnimationComponent>()) {
      auto& ac = entity.AddComponent<AnimationComponent>(SpriteManager::GetSpriteImage(SpriteType::Items));
      ac.animation = true;
      ac.is_sprite = true;
      ac.sprites.push_back(SubTexture::CreateFromCoords(ac.sprite_image, {0.0f, 18.0f}));
      ac.sprites.push_back(SubTexture::CreateFromCoords(ac.sprite_image, {1.0f, 18.0f}));
      ac.sprites.push_back(SubTexture::CreateFromCoords(ac.sprite_image, {2.0f, 18.0f}));
      ac.sprites.push_back(SubTexture::CreateFromCoords(ac.sprite_image, {3.0f, 18.0f}));
      ac.speed = 5.0f;
    }
  }
  
  void FlowerController::Update(Timestep ts) {
    CheckAndDestroy(&entity_);
  }
  void FlowerController::PreSolve(Entity* collided_entity, b2Contact* contact, const glm::vec2& contact_normal) {
    LivingEntityHitCheck(collided_entity, contact, &entity_);
  }
  
  void ScoreController::Create(Entity entity) {
    entity_ = entity;
    if (entity_.HasComponent<TextComponent>()) {
      entity_.GetComponent<TextComponent>().text = std::to_string(score_);
    }
    else {
      entity_.AddComponent<TextComponent>().text = std::to_string(score_);
    }
    
    const auto& tc = entity_.GetComponent<TransformComponent>().Translation();
    top_pos_ = {tc.x, tc.y + 2.0f};
  }
  void ScoreController::Update(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    if (tc.Translation().y < top_pos_.y) {
      tc.AddTranslation_Y(ts * speed_);
    }
    else {
      entity_.GetScene()->DestroyEntity(entity_);
    }
  }
  void ScoreController::Copy(void* script) {
    if (!script)
      return;
    
    ScoreController* score_script = reinterpret_cast<ScoreController*>(script);
    IK_ASSERT(score_script);

    top_pos_ = score_script->top_pos_;
    speed_ = score_script->speed_;
    score_ = score_script->score_;
  }
  
  struct ItemData {
    std::string name;
    std::string scrip_name;
    glm::vec2 coords;
    ScriptLoaderFn loader_fun;
    
    ItemData() = default;
    ItemData(const std::string& name, const std::string& scrip_name, glm::vec2 coords, ScriptLoaderFn fun)
    : name(name), scrip_name(scrip_name), coords(coords), loader_fun(fun) { }
  };
  
  struct RuntimeItemData {
    std::unordered_map<Items, ItemData> item_map;
  };
  static RuntimeItemData* data;
  
  void RuntimeItem::Init() {
    static auto coin_script_loader = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::CoinController") {
        sc->Bind<mario::CoinController>();
        return true;
      }
      return false;
    };
    
    static auto mushroom_script_loader = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::MushroomController") {
        sc->Bind<mario::MushroomController>();
        return true;
      }
      return false;
    };
    
    static auto flower_script_loader = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::FlowerController") {
        sc->Bind<mario::FlowerController>();
        return true;
      }
      return false;
    };

    static auto score_script_loader = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::ScoreController") {
        sc->Bind<mario::ScoreController>(100);
        return true;
      }
      return false;
    };

    data = new RuntimeItemData();
    data->item_map[Items::Coin] = { "Block Coin", "mario::CoinController", glm::vec2(0.0f, 14.0f), coin_script_loader };
    data->item_map[Items::Mushroom] = { "Mushroom", "mario::MushroomController", glm::vec2(0.0f, 19.0f), mushroom_script_loader };
    data->item_map[Items::Flower] = { "Flower", "mario::FlowerController", glm::vec2(0.0f, 18.0f), flower_script_loader };
    data->item_map[Items::Score] = { "Score", "mario::ScoreController", glm::vec2(0.0f, 0.0f), score_script_loader };
  }
  
  void RuntimeItem::Shutdown() {
    delete data;
  }
  
  void RuntimeItem::Create(Items item, EnttScene* scene, const glm::vec2& pos, int32_t score) {
    static std::shared_ptr<Texture> items = SpriteManager::GetSpriteImage(SpriteType::Items);
    
    auto run_time_entity = scene->CreateEntity(data->item_map.at(item).name);
    auto& tc = run_time_entity.GetComponent<TransformComponent>();
    tc.UpdateTranslation(glm::vec3(pos, 0.1f));
    
    if (item != Items::Score) {
      auto & qc = run_time_entity.AddComponent<QuadComponent>();
      qc.texture_comp.use = true;
      qc.texture_comp.use_sprite = true;
      qc.texture_comp.linear_edge = false;
      qc.texture_comp.component = items;
      qc.texture_comp.sprite = SubTexture::CreateFromCoords(items, data->item_map.at(item).coords);
      
      run_time_entity.AddComponent<NativeScriptComponent>(data->item_map.at(item).scrip_name, data->item_map.at(item).loader_fun);
    }
    else {
      tc.AddTranslation_Z(0.1f);
      tc.UpdateScale({0.3, 0.3, 1.0f});
      
      auto score_script_loader = [score](NativeScriptComponent* sc, const std::string& script_name) {
        if (script_name == "mario::ScoreController") {
          sc->Bind<mario::ScoreController>(score);
          return true;
        }
        return false;
      };
      run_time_entity.AddComponent<NativeScriptComponent>("mario::ScoreController", score_script_loader);
    }
  }
}
