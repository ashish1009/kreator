//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//


#pragma once

#include "layers/ikan_game_editor/src/renderer_layer.hpp"
#include "player.hpp"

namespace mario {
  
  using namespace ikan;
  
  class Mario : public ikan_game::GameData {
  public:
    Mario(const Viewport* const viewport);
    ~Mario();

    void Init() override;
    void SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) override;
    void Update(Timestep ts) override;
    void EventHandler(Event& event) override;
    void RenderGui() override;
    void SetViewportSize(uint32_t width, uint32_t height) override;

    // Getters
    std::vector<std::filesystem::path> FavDirecotries() const override;
    ImguiFont RegularFontData() const override;
    ImguiFont BoldFontData() const override;
    std::string OpenSavedScene() const override { return AM::ClientAsset("scenes/Mario_Scene.ikanScene"); }
    std::string CbpRootDir() const override { return AM::ClientAsset("scenes/"); };
    glm::vec4 GetBgColor() const override { return level_bg_; }
    std::string GameName() const override { return "Mario"; }

    static Mario& Get() { return *instance_; }

  private:
    glm::vec4 level_bg_ = {0.2, 0.4, 0.6, 1.0f};
    std::string font_path_ = "fonts/mario.ttf";

    static Mario* instance_;
  };
  
}