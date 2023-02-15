//
//  game_data.hpp
//  kreator
//
//  Created by Ashish . on 12/02/23.
//

#pragma once

namespace ikan_game {
  
  using namespace ikan;
  
  class GameData {
  public:
    virtual ~GameData() = default;
    
    virtual void Init() {}
    
    /// Initilize your game data here. This function triggers when scene in opened in game. Initialize Game renderes like Batch 2d. and Text renderer.
    /// - Parameters:
    ///   - scene: This is the reference of created scene
    ///   - panel: Scene panel pointer
    virtual void SetScene(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {};
    
    /// This funcion calls each frame in renderer Pass. Update or debug render your game.
    /// - Parameter ts: tiem step of frames
    virtual void Update(Timestep ts) {};
    
    /// This function handles the events of your game
    /// - Parameter event: event
    virtual void EventHandler(Event& event) {};
    
    /// This function renders the GUI of your game
    virtual void RenderGui() {};
    
    /// This function set the game state
    /// - Parameter is_playing: state of game
    virtual void SetState(bool is_playing) {};
    
    /// This function update the viewport width of your game
    /// - Parameters:
    ///   - width: width of viewport
    ///   - height: height of viewport
    virtual void SetViewportSize(uint32_t width, uint32_t height) {}
    
    /// This function return your viewport background color
    virtual glm::vec4 GetBgColor() { return {0.2f, 0.2f, 0.2f, 1.0f}; }
    /// This function returns name of your game
    virtual std::string GameName() {return "ikan Game";};
    /// This function returns the path of scene you want to open in begining
    virtual std::string OpenSavedScene() {return "";};
    /// This function returns the path of regular GUI fornt
    virtual ImguiFont RegularFontData() {return {AM::ProjectPath("kreator/layers/ikan_game_editor/game_assets/fonts/Opensans/Regular.ttf"), 14};};
    /// This function returns the path of bold GUI fornt
    virtual ImguiFont BoldFontData() {return {AM::ProjectPath("kreator/layers/ikan_game_editor/game_assets/fonts/Opensans/Bold.ttf"), 14};};
    /// This function returns the paths of directory you want in content browser panel
    virtual std::vector<std::filesystem::path> FavDirecotries() {return {};};
  };
  
  /// This function is the defination of game data. Implementaiton should be at client
  std::unique_ptr<GameData> CreateGameData(const Viewport* const viewport);
  
}
