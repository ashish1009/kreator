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
    void SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) override;
    
    // Getters
    std::string GameName() override;
    ImguiFont RegularFontData() override;
    ImguiFont BoldFontData() override;
    std::vector<std::filesystem::path> FavDirecotries() override;
    
  private:
    enum Direction {Left, Right, Up, Down};

    /// This function handles the Key button event
    /// - Parameter e: Key pressed event
    bool KeyPressEvent(KeyPressedEvent& e);
    /// This function render the rectangle when clicg drag mouse
    void StoreSelectedEntities();
    /// This function moves the selected entities
    /// - Parameter direction: Direction of movement
    void MoveEntities(Direction direction);
    /// This function highlight the selected entiies
    /// - Parameter enable: flag to set or unset
    void HighlightSelectedEntities(bool enable);
    /// This function clear the selected entities
    void ClearSelectedEntities();
    /// This function clear the selected entities
    void DeleteSelectedEntities();

    bool is_playing_ = false;
    const Viewport* const viewport_;
    std::shared_ptr<EnttScene> scene_;
    ScenePanelManager* panel_;
    
    std::vector<Entity*> selected_entities_;
  };
  
}
