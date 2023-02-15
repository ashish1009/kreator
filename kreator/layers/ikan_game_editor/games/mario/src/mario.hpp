//
//  mario.hpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//


#pragma once

#include "layers/ikan_game_editor/src/renderer_layer.hpp"

namespace mario {
  
  using namespace ikan;
  
  class Mario : public ikan_game::GameData {
  public:
    Mario(const Viewport* const viewport);
    ~Mario();

    void Init() override;

    static Mario& Get() { return *instance_; }

  private:
    std::string font_path_ = "fonts/mario.ttf";

    const Viewport* const viewport_;
    static Mario* instance_;
  };
  
}
