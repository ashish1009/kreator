//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"
#include "camera_controller.hpp"
#include "sprite_manager.hpp"
#include "block.hpp"
#include "runtime_item.hpp"

namespace mario {
  
  static const std::string MarioLogTag = "Mario";
  static const float game_start_pos_ = -13.0f;

  Mario* Mario::instance_ = nullptr;
  
  Mario::Mario(const Viewport* const viewport) {
    IK_ASSERT(!instance_);
    instance_ = this;

    IK_INFO(MarioLogTag, "Creating Mario Game Data ... ");
    
    // Change Text renderer Font
    TextRenderer::LoadFreetype(AM::ClientAsset(font_path_));
    BatchRenderer::Init(2000, 0, 0);
    
    SpriteManager::Init();
    BlockScriptManager::Init();
    RuntimeItem::Init();
  }
  
  Mario::~Mario() {
    IK_WARN(MarioLogTag, "Destroying Mario Game Data ... ");

    SpriteManager::Shutdown();
    BlockScriptManager::Shutdown();
    RuntimeItem::Shutdown();
  }
    
  void Mario::Init(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
    IK_INFO(MarioLogTag, "Initializing Mario Game Data ... ");

    scene_ = scene;
    panel_ = panel;

    CreateOrSearchCamera();
    SearchOrCreatePlayer();
    AddScriptsToEntities();
  }
  
  void Mario::Update(Timestep ts) {
  }
  
  void Mario::RenderGui() {
  }
  
  void Mario::EventHandler(Event& event) {
  }
  
  void Mario::SetViewportSize(uint32_t width, uint32_t height) {
  }

  ImguiFont Mario::RegularFontData() const {
    return { AM::ClientAsset(font_path_), 14.0f };
  }
  
  ImguiFont Mario::BoldFontData() const {
    return { AM::ClientAsset(font_path_), 14.0f };
  }

  std::vector<std::filesystem::path> Mario::FavDirecotries() const {
    return {
      AM::ProjectPath("kreator/layers/ikan_game_editor/games/mario/assets"),
      AM::ClientAsset("scenes"),
    };
  }

  void Mario::CreateOrSearchCamera() {
    bool found_camera = false;
    Entity camera_entity;

    auto camera_view = scene_->GetEntitesWith<CameraComponent>();
    for (auto entity : camera_view) {
      auto& camera_comp = camera_view.get<CameraComponent>(entity);
      if (camera_comp.is_primary == true) {
        found_camera = true;
        camera_entity = Entity(entity, scene_.get());
        break;
      }
    }
    
    if (!found_camera) {
      camera_entity = scene_->CreateEntity("Mario Primary Camera");
      auto& cc = camera_entity.AddComponent<CameraComponent>();
      cc.is_primary = true;
    }
    
    camera_entity.GetComponent<TransformComponent>().UpdateTranslation_X(game_start_pos_);

    // Mario Camera controler script loader function
    auto camera_controler_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::CameraController") {
        sc->Bind<mario::CameraController>();
        return true;
      }
      return false;
    };

    // Add Native script
    if (camera_entity.HasComponent<NativeScriptComponent>()) {
      auto& nsc = camera_entity.GetComponent<NativeScriptComponent>();
      nsc.loader_function = camera_controler_loader_fn;
      nsc.Bind<CameraController>();
    }
    else {
      camera_entity.AddComponent<NativeScriptComponent>("mario::CameraController", camera_controler_loader_fn);
    }
  }

  void Mario::SearchOrCreatePlayer() {
    const std::string player_name = "Mario Player";
    bool found_player = false;
    Entity player_entity;
  
    auto player_view = scene_->GetEntitesWith<TagComponent>();
    for (auto entity : player_view) {
      auto& player_tag = player_view.get<TagComponent>(entity).tag;
      // - Note: This has to be the payer Name in the game
      if (player_tag == player_name) {
        found_player = true;
        player_entity = Entity(entity, scene_.get());
        break;
      }
    }

    if (!found_player) {
      player_entity = scene_->CreateEntity(player_name);
    }

    player_entity.GetComponent<TransformComponent>().UpdateTranslation_X(game_start_pos_);

    // Quad Component
    if (!player_entity.HasComponent<QuadComponent>()) {
      auto& qc = player_entity.AddComponent<QuadComponent>();
      qc.texture_comp.use = true;
      qc.texture_comp.use_sprite = true;
      qc.texture_comp.linear_edge = false;
      qc.texture_comp.component = SpriteManager::GetSpriteImage(SpriteType::Player);
      qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(PlayerState::Small, PlayerAction::Idle).at(0); 
    }
    
    // Rigid Body Component
    if (!player_entity.HasComponent<RigidBodyComponent>()) {
      auto& rb = player_entity.AddComponent<RigidBodyComponent>();
      rb.type = b2_dynamicBody;
      rb.fixed_rotation = true;
    }
    
    // Collider Component
    if (!player_entity.HasComponent<PillBoxColliderComponent>()) {
      auto& pbc = player_entity.AddComponent<PillBoxColliderComponent>();
      pbc.width = 0.4f;
    }
    
    // Native script
    auto player_controler_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::PlayerController") {
        sc->Bind<mario::PlayerController>();
        return true;
      }
      return false;
    };
    if (!player_entity.HasComponent<NativeScriptComponent>()) {
      player_entity.AddComponent<NativeScriptComponent>("mario::PlayerController", player_controler_loader_fn);
    }
    else {
      auto& nsc = player_entity.GetComponent<NativeScriptComponent>();
      nsc.loader_function = player_controler_loader_fn;
      nsc.Bind<PlayerController>();
    }
    
    player_controller_ = PlayerController::Get();
  }
  
  void Mario::AddScriptsToEntities() {
    auto tag_view = scene_->GetEntitesWith<TagComponent>();
    
    for (auto e : tag_view) {
      const auto &c = tag_view.get<TagComponent>(e);
      if (IsBlock(c.tag)) {
        Entity brick_entity = Entity(e, scene_.get());
        if (brick_entity.HasComponent<NativeScriptComponent>()) {
          auto& nsc = brick_entity.GetComponent<NativeScriptComponent>();
          
          nsc.loader_function = BSM::GetLoaderFn(c.tag);
          nsc.Bind<BlockController>(BSM::GetType(c.tag), BSM::GetCount(c.tag));
        }
        else {
          brick_entity.AddComponent<NativeScriptComponent>("mario::BlockController",
                                                           BSM::GetLoaderFn(c.tag)).Bind<BlockController>(BSM::GetType(c.tag),
                                                                                                          BSM::GetCount(c.tag));
        }
      }
    }
  }
  
}
