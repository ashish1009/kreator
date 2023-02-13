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

  struct ItemData {
    std::string name;
    glm::vec2 coords;
    ScriptLoaderFn loader_fun;
    
    ItemData() = default;
    ItemData(const std::string& name, glm::vec2 coords, ScriptLoaderFn fun)
    : name(name), coords(coords), loader_fun(fun) {
      
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
    
    data = new RuntimeItemData();
    data->item_map[Items::Coin] = { "Block Coin", glm::vec2(0.0f, 14.0f), coin_script_loader };
  }
  
  void RuntimeItem::Shutdown() {
    delete data;
  }
  
  void RuntimeItem::Create(Items item, EnttScene* scene, const glm::vec2& pos) {
    static std::shared_ptr<Texture> icons = SpriteManager::GetSpriteImage(SpriteType::Items);
    
    auto coin_entity = scene->CreateEntity(data->item_map.at(item).name);
    coin_entity.GetComponent<TransformComponent>().UpdateTranslation(glm::vec3(pos, 0.1f));
    
    auto & qc = coin_entity.AddComponent<QuadComponent>();
    qc.texture_comp.use = true;
    qc.texture_comp.use_sprite = true;
    qc.texture_comp.linear_edge = false;
    qc.texture_comp.component = icons;
    qc.texture_comp.sprite = SubTexture::CreateFromCoords(icons, data->item_map.at(item).coords);
    
    coin_entity.AddComponent<NativeScriptComponent>("mario::CoinController", data->item_map.at(item).loader_fun);
  }
  
}
