//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#pragma once

#include "renderer_layer.hpp"

namespace mario {
  
  using namespace ikan;
    
  class MarioData : public ikan_game::GameData {
  public:
    MarioData(const Viewport* const viewport);
    
    void Init() override;
    void Update(Timestep ts) override;
    void EventHandler(Event& event) override;
    void RenderGui() override;
    
    void SetState(bool is_playing) override;
    void SetScene(const std::shared_ptr<EnttScene> scene) override;
    
    // Getters
    std::string GameName() override;
    ImguiFont RegularFontData() override;
    ImguiFont BoldFontData() override;
    std::vector<std::filesystem::path> FavDirecotries() override;
    
  private:
    /// This function handles the mouse button event
    /// - Parameter e: mouse button pressed event
    bool MouseButtonPressed(MouseButtonPressedEvent& e);

    bool is_playing_ = false;
    const Viewport* const viewport_;
    std::shared_ptr<EnttScene> scene_;
  };
  
}
