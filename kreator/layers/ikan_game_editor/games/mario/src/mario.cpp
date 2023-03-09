//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"
#include "player.hpp"
#include "sprite_manager.hpp"
#include "block.hpp"
#include "runtime_item.hpp"
#include "enemy.hpp"

namespace mario {
  
  static const std::string MarioLogTag = "Mario";

  Mario* Mario::instance_ = nullptr;
  
  Mario::Mario(const Viewport* const viewport) {
    IK_ASSERT(!instance_);
    instance_ = this;

    IK_INFO(MarioLogTag, "Creating Mario Game Data ... ");
  }
  
  Mario::~Mario() {
    IK_WARN(MarioLogTag, "Destroying Mario Game Data ... ");
  }
  
  void Mario::Init() {
    IK_INFO(MarioLogTag, "Initializing Mario Game Data ... ");
  }
  
  void Mario::SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
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

}
