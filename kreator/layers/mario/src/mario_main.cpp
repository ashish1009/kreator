//
//  mario_main.cpp
//  mario
//
//  Created by Ashish . on 02/01/23.
//

#include "mario_main.hpp"
#include "camera_controller.h"

namespace mario {
  
  static glm::mat4 still_camera_projection;
  static float score_pos_row_1_y = 0.0f;
  static float score_pos_row_2_y = 0.0f;
  
  static float score_pos_col_1_y = 50.0f;
  static float score_pos_col_2_y = 50.0f;
  static float score_pos_col_3_y = 50.0f;
  static float score_pos_col_4_y = 50.0f;

  MarioLayer::MarioLayer() : Layer("Kreator") {
    IK_INFO("Mario", "Creating Mario Layer instance ... ");
    
    viewport_width_ = Application::Get().GetSpecification().window_specification.width;
    viewport_height_ = Application::Get().GetSpecification().window_specification.height;
    
    still_camera_projection = glm::ortho( 0.0f, (float)viewport_width_, 0.0f, (float)viewport_height_);

    score_pos_row_1_y = (float)viewport_height_ - 50;
    score_pos_row_2_y = (float)viewport_height_ - 70;
    
    score_pos_col_1_y = 50.0f;
    score_pos_col_2_y = ((float)viewport_width_ / 2.0f) - 50.0f;
    score_pos_col_3_y = ((float)viewport_width_ / 2.0f) + 50.0f;
    score_pos_col_4_y = (float)viewport_width_ - 50.0f;


    // Reinitialize the Batch Renderer
    BatchRenderer::Reinit(1000, 0, 0);
    
#if MARIO_DEBUG
    use_sprite_ = true;
    if (use_sprite_)
      spm_.SetSceneContext(&mario_tile_scene_);
    else
      spm_.SetSceneContext(&mario_texture_scene_);
#else
    use_sprite_ = false;
#endif
  }
  
  MarioLayer::~MarioLayer() {
    IK_WARN("Mario", "Destroying Mario Layer instance !!! ");
  }
  
  void MarioLayer::CreateCamera(EnttScene* scene, Entity& camera_entity) {
    camera_entity = scene->CreateEntity("Camera");
    camera_entity.GetComponent<TransformComponent>().translation.y = 2.0f;
    {
      auto& camera_comp = camera_entity.AddComponent<CameraComponent>();
      camera_comp.is_primary = true;
      camera_comp.camera->SetOrthographicSize(22.0f);
      
      camera_entity.AddComponent<NativeScriptComponent>([](NativeScriptComponent* sc,
                                                           const std::string& script_name) {
        if (script_name == "mario::CameraController") {
          sc->Bind<mario::CameraController>(10.0f);
          return true;
        }
        return false;
      }).Bind<CameraController>(player_data::speed_);
    }
  }
  
  void MarioLayer::Attach() {
    IK_INFO("Mario", "Attaching Mario Layer instance");
    
    // Change Text renderer Font
    TextRenderer::LoadFreetype(AM::ClientAsset("fonts/mario.ttf"));

    // ---------------------------------------------------------
    // Set the scene as playing
    // ---------------------------------------------------------
    mario_texture_scene_.PlayScene();
    mario_tile_scene_.PlayScene();

    // ---------------------------------------------------------
    // Create the camera entity
    // ---------------------------------------------------------
    CreateCamera(&mario_tile_scene_, tile_camera_entity_);
    CreateCamera(&mario_texture_scene_, texture_camera_entity);

    // --------------------------------------------------------
    // Player
    // --------------------------------------------------------
    player_ = new Player(&mario_tile_scene_);

    // ---------------------------------------------------------
    // Create memory for background data
    // ---------------------------------------------------------
    background_data_ = new BackgroudData(&mario_tile_scene_, &mario_texture_scene_,
                                         Renderer::GetTexture(AM::ClientAsset("textures/tiles.png"), false));

  }
  
  void MarioLayer::Detach() {
    IK_WARN("Mario", "Detaching Mario Layer instance ");
    delete background_data_;
    delete player_;
  }
  
  void MarioLayer::Render(Timestep ts) {
    if (use_sprite_) {
      mario_tile_scene_.Update(ts);
    } else {
      mario_texture_scene_.Update(ts);

      static SceneCamera fixed_camera;
      static std::shared_ptr<Texture> bg_texture = Renderer::GetTexture(AM::ClientAsset("textures/background/background.png"));

      BatchRenderer::BeginBatch(fixed_camera.GetProjection());
      BatchRenderer::DrawQuad(Math::GetTransformMatrix({0, -0, -0.5}, {0, 0, 0}, {18, 10, 1}), bg_texture);
      BatchRenderer::EndBatch();
    }
    
    // Score and All text
    {
      TextRenderer::RenderText("MARIO",
                               still_camera_projection,
                               { score_pos_col_1_y, score_pos_row_1_y, 0.3f },
                               { 0.6f, 0.6f },
                               { 1.0f, 1.0f, 1.0f, 1.0f });
      TextRenderer::RenderText("WORLD",
                               still_camera_projection,
                               { score_pos_col_2_y, score_pos_row_1_y, 0.3f },
                               { 0.6f, 0.6f },
                               { 1.0f, 1.0f, 1.0f, 1.0f });

      TextRenderer::RenderText("WORLD",
                               still_camera_projection,
                               { score_pos_col_3_y, score_pos_row_1_y, 0.3f },
                               { 0.6f, 0.6f },
                               { 1.0f, 1.0f, 1.0f, 1.0f });

      TextRenderer::RenderText("TIME",
                               still_camera_projection,
                               { score_pos_col_4_y, score_pos_row_1_y, 0.3f },
                               { 0.6f, 0.6f },
                               { 1.0f, 1.0f, 1.0f, 1.0f });
    }
    
    /// Render the Frame rate
    TextRenderer::RenderText(std::to_string((uint32_t)(ImGui::GetIO().Framerate)),
                             still_camera_projection,
                             { 5.0f, 5.0f, 0.3f },
                             { 0.35f, 0.35f },
                             { 0, 0, 1, 1 });
  }
  
  void MarioLayer::Update(Timestep ts) {
#if MARIO_DEBUG
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      // TODO: Store the player position before resize and back it up after resize
      mario_tile_scene_.SetViewport(viewport_.width, viewport_.height);
      mario_texture_scene_.SetViewport(viewport_.width, viewport_.height);

      still_camera_projection = glm::ortho( 0.0f, (float)viewport_.width, 0.0f, (float)viewport_.height);
      score_pos_row_1_y = (float)viewport_.height - 50;
      score_pos_row_2_y = (float)viewport_.height - 70;
      
      score_pos_col_2_y = ((float)viewport_.width / 2.0f) - 250.0f;
      score_pos_col_3_y = ((float)viewport_.width / 2.0f) + 150.0f;
      score_pos_col_4_y = (float)viewport_.width - 150.0f;

    }

    viewport_.UpdateMousePos();
    viewport_.framebuffer->Bind();

    Renderer::Clear(viewport_.framebuffer->GetSpecification().color);
    Render(ts);

    viewport_.UpdateHoveredEntity(spm_.GetSelectedEntity(), &mario_texture_scene_);
    viewport_.framebuffer->Unbind();
#else
    Renderer::Clear({0.2, 0.3, 0.4, 1.0});
    Render(ts);
    
#endif
  }
  
  void MarioLayer::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(MarioLayer::OnWindowResized));
#if MARIO_DEBUG
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(MarioLayer::OnMouseButtonPressed));
#endif
  }
  
  bool MarioLayer::OnWindowResized(WindowResizeEvent &e) {
    viewport_width_ = e.GetWidth();
    viewport_height_ = e.GetHeight();
    
    // TODO: Store the player position before resize and back it up after resize
    mario_tile_scene_.SetViewport(viewport_width_, viewport_height_);
    mario_texture_scene_.SetViewport(viewport_width_, viewport_height_);

    still_camera_projection = glm::ortho( 0.0f, (float)viewport_width_, 0.0f, (float)viewport_height_);

    score_pos_row_1_y = (float)viewport_height_ - 50;
    score_pos_row_2_y = (float)viewport_height_ - 70;

    score_pos_col_2_y = ((float)viewport_width_ / 2.0f) - 50.0f;
    score_pos_col_3_y = ((float)viewport_width_ / 2.0f) + 50.0f;
    score_pos_col_4_y = (float)viewport_width_ - 50.0f;

    return false;
  }
  
#if MARIO_DEBUG
  bool MarioLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and
          viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and
          viewport_.mouse_pos_y <= viewport_.height) {
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
      }
    }
    return false;
  }
#endif
  
  void MarioLayer::RenderGui() {
#if MARIO_DEBUG
    ImguiAPI::StartDcocking();
    Renderer::RenderStatsGui(nullptr, true);
    Renderer::Framerate();

    viewport_.RenderGui();
    if (use_sprite_)
      mario_tile_scene_.RenderGui();
    else
      mario_texture_scene_.RenderGui();

    ImGui::Begin("Setting");
    ImGui::PushID("Setting");
    
    if (ImGui::Button("ToggleScene")) {
      use_sprite_ = use_sprite_ ? false : true ;
      
      if (use_sprite_)
        spm_.SetSceneContext(&mario_tile_scene_);
      else
        spm_.SetSceneContext(&mario_texture_scene_);
    }
    ImGui::SameLine();
        
    if (ImGui::Button("Reset")) {
      player_->Reset();
      tile_camera_entity_.GetComponent<TransformComponent>().translation.x = 0.0f;
      texture_camera_entity.GetComponent<TransformComponent>().translation.x = 0.0f;
    }
    
    ImGui::PopID();
    ImGui::End();
    
    spm_.RenderGui();
    player_->RenderGui();

    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Mario");
    ImGui::PushID("Mario");
    
    viewport_.focused = ImGui::IsWindowFocused();
    viewport_.hovered = ImGui::IsWindowHovered();
    
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    viewport_.width = viewport_panel_size.x;
    viewport_.height = viewport_panel_size.y;
    
    size_t textureID = viewport_.framebuffer->GetColorAttachmentIds().at(0);
    ImGui::Image((void*)textureID,
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    viewport_.UpdateBound();
        
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();

    ImguiAPI::EndDcocking();
#endif
  }
  
}
