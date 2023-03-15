//
//  game_data.h
//  kreator
//
//  Created by Ashish . on 13/03/23.
//

#pragma once

namespace kreator {
  
  using namespace ikan;
  
  enum class GameType {
    None, Mario
  };

  class GameData {
  public:
    virtual ~GameData() = default;
    /// This function returns name of your game
    virtual std::string GameName() const = 0;
    /// This function returns the paths of directory you want in content browser panel
    virtual std::string CbpRootDir() const = 0;
    /// This function returns the paths of directory you want in content browser panel
    virtual std::vector<std::filesystem::path> FavDirecotries() const = 0;
    /// This function returns the path of regular GUI fornt
    virtual ImguiFont RegularFontData() const = 0;
    /// This function returns the path of bold GUI fornt
    virtual ImguiFont BoldFontData() const = 0;
  };
  
  class GameDataImpl : public GameData {
  public:
    std::string GameName() const override { return "Kreator Editor"; }
    std::string CbpRootDir() const override { return "../../../kreator/layers/ikan_game_editor/"; };
    virtual std::vector<std::filesystem::path> FavDirecotries() const override { return {}; };
    virtual ImguiFont RegularFontData() const override {
      return {AM::ProjectPath("kreator/layers/ikan_game_editor/game_assets/fonts/Opensans/Regular.ttf"), 14};
    };
    virtual ImguiFont BoldFontData() const override {
      return {AM::ProjectPath("kreator/layers/ikan_game_editor/game_assets/fonts/Opensans/Bold.ttf"), 14};
    };
  };

  /// This function is the defination of game data. Implementaiton should be at client
  std::unique_ptr<GameData> CreateGameData(GameType game_type);
  
}
