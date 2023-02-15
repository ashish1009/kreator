//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"

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
  }
  
  void Mario::Init() {
    IK_INFO(MarioLogTag, "Initializing Mario Game Data ... ");

    // Change Text renderer Font
    TextRenderer::LoadFreetype(AM::ClientAsset(font_path_));
    BatchRenderer::Init(2000, 0, 0);
  }
  
  void Mario::SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
  }
  
  void Mario::Update(Timestep ts) {
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
