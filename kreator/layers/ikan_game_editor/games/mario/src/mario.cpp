//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"

#include "SpriteManager.hpp"

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
  }
  
  void Mario::Update(Timestep ts) {
    if (!is_playing_) {
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
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(Mario::MouseButtonPressEvent));
  }
  
  bool Mario::KeyPressEvent(KeyPressedEvent &e) {
    return false;
  }
  
  bool Mario::MouseButtonPressEvent(MouseButtonPressedEvent& e) {
    return false;
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
