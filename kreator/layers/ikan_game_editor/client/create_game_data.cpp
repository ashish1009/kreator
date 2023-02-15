//
//  create_game_data.cpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#include "layers/ikan_game_editor/src/renderer_layer.hpp"
#include "layers/ikan_game_editor/games/mario/src/mario.hpp"

std::unique_ptr<ikan_game::GameData> ikan_game::CreateGameData(GameType type, const ikan::Viewport* const viewport) {
  switch (type) {
    case ikan_game::GameType::Editor: return std::make_unique<ikan_game::GameData>();
    case ikan_game::GameType::Mario: return std::make_unique<mario::Mario>();
    IK_ASSERT(false);
  }
}
