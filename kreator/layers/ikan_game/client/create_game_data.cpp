//
//  create_game_data.cpp
//  kreator
//
//  Created by Ashish . on 08/02/23.
//

#include "layers/ikan_game/src/renderer_layer.hpp"
#include "layers/mario/new_game/mario.hpp"

std::unique_ptr<ikan_game::GameData> ikan_game::CreateGameData(const ikan::Viewport* const viewport) {
  return std::make_unique<mario::MarioData>(viewport);
}
