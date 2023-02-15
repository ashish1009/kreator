//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"
#include "player.hpp"
#include "sprite_manager.hpp"

namespace mario {
  
  static const std::string MarioLogTag = "Mario";

  Mario* Mario::instance_ = nullptr;
  
  Mario::Mario(const Viewport* const viewport)
  : viewport_(viewport) {
    IK_ASSERT(!instance_);
    instance_ = this;

    IK_INFO(MarioLogTag, "Creating Mario Game Data ... ");
  }
  
  Mario::~Mario() {
    IK_WARN(MarioLogTag, "Destroying Mario Game Data ... ");
    
    SpriteManager::Shutdown();
  }
  
  void Mario::Init() {
    IK_INFO(MarioLogTag, "Initializing Mario Game Data ... ");

    // Change Text renderer Font
    TextRenderer::LoadFreetype(AM::ClientAsset(font_path_));
    BatchRenderer::Init(2000, 0, 0);
    
    SpriteManager::Init();
  }
  
  void Mario::SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
    scene_ = scene;
    panel_ = panel;
    
    SearchOrCreatePlayer();
  }
  
  void Mario::Update(Timestep ts) {
    if (!is_playing_) {
      SelectEntities();
      MoveCameraDebug(ts);
    }
    else {
      // Timer
      timer_ += ts;
      time_left_ = MaxTime - (uint32_t)timer_;
    }
    
    // Score and All text
    TextRenderer::BeginBatch(text_data_.still_camera_projection);
    
    text_data_.Render("MARIO", 0, 0);
    text_data_.Render(std::to_string(score_), 1, 0);
    
    BatchRenderer::BeginBatch(text_data_.still_camera_projection);
    BatchRenderer::DrawQuad(Math::GetTransformMatrix({0, 0, 0}, {0, 0, 0}, {10, 10, 10}), {1, 1,1 ,1});
    BatchRenderer::EndBatch();
    text_data_.Render("x " + std::to_string(coins_), 1, 1);
    
    text_data_.Render("WORLD", 0, 2);
    text_data_.Render(std::to_string(world_) + " - " + std::to_string(level_), 1, 2);
    
    text_data_.Render("TIME", 0, 3);
    text_data_.Render(std::to_string(time_left_), 1, 3);
    
    TextRenderer::RenderText(std::to_string((uint32_t)(ImGui::GetIO().Framerate)), { 5.0f, 5.0f, 0.3f }, { 0.35f, 0.35f }, { 0, 0, 1, 1 });
    
    TextRenderer::EndBatch();
  }
  
  void Mario::RenderGui() {
  }
  
  void Mario::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(Mario::KeyPressEvent));
  }
  
  bool Mario::KeyPressEvent(KeyPressedEvent &e) {
    if (!is_playing_) {
      bool shift = Input::IsKeyPressed(KeyCode::LeftShift);
      if (shift) {
        switch (e.GetKeyCode()) {
          case KeyCode::D:          DuplicateSelectedEntities();  break;
          case KeyCode::Backspace:  DeleteSelectedEntities();     break;
          case KeyCode::Escape:     ClearSelectedEntities();      break;
            
          case KeyCode::Left:     MoveEntities(Left);   break;
          case KeyCode::Right:    MoveEntities(Right);  break;
          case KeyCode::Up:       MoveEntities(Up);     break;
          case KeyCode::Down:     MoveEntities(Down);   break;
            
          default: break;
        } // switch (e.GetKeyCode())
      } // if (shift)
    }
    return false;
  }
    
  void Mario::SelectEntities() {
    if (!(viewport_->mouse_pos_x >= 0 and viewport_->mouse_pos_y >= 0 and
          viewport_->mouse_pos_x <= viewport_->width and viewport_->mouse_pos_y <= viewport_->height)) {
      ClearSelectedEntities();
      return;
    }
    
    if (!Input::IsKeyPressed(KeyCode::LeftShift) or Input::IsKeyPressed(KeyCode::LeftControl))
      return;
    
    static bool first_clicked = true;
    
    static glm::vec2 initial_mouse_position_ = glm::vec2(0.0f);
    static glm::vec2 final_mouse_position_ = glm::vec2(0.0f);
    
    static glm::vec2 initial_block_position_ = glm::vec2(0.0f);
    static glm::vec2 final_block_position_ = glm::vec2(0.0f);
    
    const auto& cd = scene_->GetPrimaryCameraData();
    float zoom = cd.scene_camera->GetZoom();
    float aspect_ratio = cd.scene_camera->GetAspectRatio();
    
    if (Input::IsMouseButtonPressed(MouseButton::ButtonLeft)) {
      if (first_clicked) {
        ClearSelectedEntities();
        
        first_clicked = false;
        initial_mouse_position_ = { viewport_->mouse_pos_x, viewport_->mouse_pos_y };
        initial_block_position_ = {
          viewport_->mouse_pos_x - ((float)viewport_->width / 2),
          viewport_->mouse_pos_y - ((float)viewport_->height / 2)
        };
        initial_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
      }
      
      final_mouse_position_ = { viewport_->mouse_pos_x, viewport_->mouse_pos_y };
      final_block_position_ = {
        viewport_->mouse_pos_x - ((float)viewport_->width / 2),
        viewport_->mouse_pos_y - ((float)viewport_->height / 2)
      };
      final_block_position_ *= ((zoom * aspect_ratio) / viewport_->width);
      
      // Render the outline rectangle
      BatchRenderer::BeginBatch(scene_->GetPrimaryCameraData().scene_camera->GetProjection() *
                                glm::inverse(scene_->GetPrimaryCameraData().transform_matrix));
      BatchRenderer::DrawRect({initial_block_position_.x + cd.position.x, initial_block_position_.y + cd.position.y, 0.1},
                              {final_block_position_.x + cd.position.x, final_block_position_.y + cd.position.y, 0.1},
                              {1, 1, 1, 1});
      BatchRenderer::EndBatch();
      
    }
    if (Input::IsMouseButtonReleased(MouseButton::ButtonLeft)) {
      if (!first_clicked) {
        // Store entites present in selected entitity
        float min_x = std::min(initial_mouse_position_.x, final_mouse_position_.x);
        float max_x = std::max(initial_mouse_position_.x, final_mouse_position_.x);
        float min_y = std::min(initial_mouse_position_.y, final_mouse_position_.y);
        float max_y = std::max(initial_mouse_position_.y, final_mouse_position_.y);
        
        for (float i_x = min_x; i_x <= max_x; i_x ++) {
          for (float i_y = min_y; i_y <= max_y; i_y++) {
            // Get pixel from rednerer
            int32_t pixel = -1;
            
            Renderer::GetEntityIdFromPixels(i_x, i_y, viewport_->framebuffer->GetPixelIdIndex(), pixel);
#if 0
            MARIO_TRACE("X : {0}, Y : {1}, Pixel : {2}", i_x, i_y, pixel);
#endif
            
            if (scene_) {
              if (pixel <= (int32_t)scene_->GetMaxEntityId()) {
                if (selected_entities_.find((entt::entity)pixel) == selected_entities_.end()){
                  selected_entities_[(entt::entity)pixel] = scene_->GetEnitityFromId(pixel);
                }
              }
            }
          }
        }
        
        // TODO: Do it while adding to vector? or keep separate???
        HighlightSelectedEntities(true);
      }
      first_clicked = true;
    }
  }
  
  void Mario::MoveEntities(Direction direction) {
    for (auto& [entt, entity] : selected_entities_) {
      if(!entity) continue;
      
      auto& tc = entity->GetComponent<TransformComponent>();
      switch (direction) {
        case Down:      tc.UpdateTranslation_Y(tc.Translation().y - 1.0f);     break;
        case Up:        tc.UpdateTranslation_Y(tc.Translation().y + 1.0f);     break;
        case Right:     tc.UpdateTranslation_X(tc.Translation().x + 1.0f);     break;
        case Left:      tc.UpdateTranslation_X(tc.Translation().x - 1.0f);     break;
        default: break;
      }
    }
  }

  void Mario::DuplicateSelectedEntities() {
    HighlightSelectedEntities(false);
    for (auto& [entt, entity] : selected_entities_) {
      scene_->DuplicateEntity(*entity);
    }
    HighlightSelectedEntities(true);
  }
  
  void Mario::ClearSelectedEntities() {
    HighlightSelectedEntities(false);
    selected_entities_.clear();
  }
  
  void Mario::DeleteSelectedEntities() {
    for (auto& [entt, entity] : selected_entities_) {
      if (panel_->GetSelectedEntity() and *(panel_->GetSelectedEntity()) == *entity) {
        panel_->SetSelectedEntity(nullptr);
      }
      scene_->DestroyEntity(*entity);
    }
    selected_entities_.clear();
  }
  
  void Mario::HighlightSelectedEntities(bool enable) {
    for (auto& [entt, entity] : selected_entities_) {
      if(!entity) continue;
      
      auto& tc = entity->GetComponent<TransformComponent>();
      auto& qc = entity->GetComponent<QuadComponent>();
      
      if (enable) {
        tc.UpdateTranslation_Z(0.1f);
        qc.color.a -=0.2f;
      }
      else {
        tc.UpdateTranslation_Z(0.0f);
        qc.color.a +=0.2f;
      }
    }
  }
  
  void Mario::MoveCameraDebug(Timestep ts) {
    // Move Camera for debug
    auto& cd = scene_->GetPrimaryCameraData();
    auto& cam = cd.scene_camera;
    auto& tc = cd.transform_comp;
    
    bool shift = Input::IsKeyPressed(KeyCode::RightShift);
    if (shift) {
      if (Input::IsKeyPressed(KeyCode::A)) tc->UpdateTranslation_X(tc->Translation().x - (cam->GetZoom() * ts));
      if (Input::IsKeyPressed(KeyCode::D)) tc->UpdateTranslation_X(tc->Translation().x + (cam->GetZoom() * ts));
      
      if (Input::IsKeyPressed(KeyCode::W)) tc->UpdateTranslation_Y(tc->Translation().y + (cam->GetZoom() * ts));
      if (Input::IsKeyPressed(KeyCode::S)) tc->UpdateTranslation_Y(tc->Translation().y - (cam->GetZoom() * ts));
      
      if (Input::IsKeyPressed(KeyCode::Q)) cam->SetOrthographicSize(cam->GetOrthographicSize() + 1.0f);
      if (Input::IsKeyPressed(KeyCode::E)) cam->SetOrthographicSize(cam->GetOrthographicSize() - 1.0f);
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
    
    // Player should be in front
    player_entity.GetComponent<TransformComponent>().UpdateTranslation_Z(0.1f);

    // Quad Component
    if (!player_entity.HasComponent<QuadComponent>()) {
      auto& qc = player_entity.AddComponent<QuadComponent>();
      qc.texture_comp.use = true;
      qc.texture_comp.use_sprite = true;
      qc.texture_comp.linear_edge = false;
      qc.texture_comp.component = SpriteManager::GetSpriteImage(SpriteType::Player);
      qc.texture_comp.sprite = SpriteManager::GetPlayerStateSprite(PlayerState::Small,
                                                                  PlayerAction::Idle).at(0); // As we know only one image for idle state
    }
    
    // Rigid Body Component
    if (!player_entity.HasComponent<RigidBodyComponent>()) {
      auto& rb = player_entity.AddComponent<RigidBodyComponent>();
      rb.type = b2_dynamicBody;
      rb.fixed_rotation = true;
    }

    // Collider Component
    if (!player_entity.HasComponent<PillBoxColliderComponent>()) {
      auto& pbc = player_entity.AddComponent<PillBoxColliderComponent>(player_entity);
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
  }
  
  void Mario::SetViewportSize(uint32_t width, uint32_t height) {
    viewport_width_ = width;
    viewport_height_ = height;
    
    text_data_.Update(viewport_width_, viewport_height_);
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

}
