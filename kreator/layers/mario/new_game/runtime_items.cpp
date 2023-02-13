//
//  coin.cpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#include "runtime_items.hpp"
#include "sprite_manager.hpp"

namespace mario {

  void CoinController::Create(Entity entity) {
    entity_ = entity;
    const auto& tc = entity_.GetComponent<TransformComponent>().Translation();
    top_pos_ = {tc.x, tc.y + 5.0f};
  }
  
  void CoinController::Update(Timestep ts) {
    auto& tc = entity_.GetComponent<TransformComponent>();
    if (tc.Translation().y < top_pos_.y) {
      tc.UpdateTranslation_Y(tc.Translation().y + ts * speed_);
      tc.UpdateScale_X(tc.Scale().x - fmod((2.5f * ts), -1.0f));
    }
    else {
      entity_.GetScene()->DestroyEntity(entity_);
    }
  }
  
  void MushroomController::Create(Entity entity) {
    entity_ = entity;
    rigid_body_comp_ = &(entity_.AddComponent<RigidBodyComponent>());
    rigid_body_comp_->type = b2_dynamicBody;
    rigid_body_comp_->angular_velocity = 0.0f;
    rigid_body_comp_->fixed_rotation = true;
    
    auto& bcc = entity.AddComponent<BoxColloiderComponent>();
    bcc.runtime_fixture = &entity_;
    
    entity_.GetScene()->AddBodyToPhysicsWorld(entity_, *rigid_body_comp_);
  }
  
  void MushroomController::Update(Timestep ts) {
    static const float inner_player_width = 0.6f;
    static const float y_val = -(0.60f);
    
    bool on_ground_ = entity_.GetScene()->CheckOnGround(&entity_, inner_player_width, y_val);
    
    if (on_ground_) {
      velocity_.x = 8.0f;
      velocity_.y = 0;
    }
    else {
      velocity_.x = 0.0f;
      velocity_.y = entity_.GetScene()->GetPhysicsWorld()->GetGravity().y * 2.7f;
    }

    rigid_body_comp_->SetVelocity({8.0f, -10.0f});
  }

  struct ItemData {
    std::string name;
    std::string scrip_name;
    glm::vec2 coords;
    ScriptLoaderFn loader_fun;
    
    ItemData() = default;
    ItemData(const std::string& name, const std::string& scrip_name, glm::vec2 coords, ScriptLoaderFn fun)
    : name(name), scrip_name(scrip_name), coords(coords), loader_fun(fun) {
      
    }
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
    
    data = new RuntimeItemData();
    data->item_map[Items::Coin] = { "Block Coin", "mario::CoinController", glm::vec2(0.0f, 14.0f), coin_script_loader };
    data->item_map[Items::Mushroom] = { "Mushroom", "mario::MushroomController", glm::vec2(0.0f, 20.0f), mushroom_script_loader };
  }
  
  void RuntimeItem::Shutdown() {
    delete data;
  }
  
  void RuntimeItem::Create(Items item, EnttScene* scene, const glm::vec2& pos) {
    static std::shared_ptr<Texture> items = SpriteManager::GetSpriteImage(SpriteType::Items);
    
    auto coin_entity = scene->CreateEntity(data->item_map.at(item).name);
    coin_entity.GetComponent<TransformComponent>().UpdateTranslation(glm::vec3(pos, 0.1f));
    
    auto & qc = coin_entity.AddComponent<QuadComponent>();
    qc.texture_comp.use = true;
    qc.texture_comp.use_sprite = true;
    qc.texture_comp.linear_edge = false;
    qc.texture_comp.component = items;
    qc.texture_comp.sprite = SubTexture::CreateFromCoords(items, data->item_map.at(item).coords);
    
    coin_entity.AddComponent<NativeScriptComponent>(data->item_map.at(item).scrip_name, data->item_map.at(item).loader_fun);
  }
  
}
