//
//  entt_scene.hpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#pragma once

#include <entt.hpp>

namespace ikan {

  class EnttScene {
  public:
    /// This Constructor creates the instance of Scene.
    /// - Parameter file_path: optional file path if we want to create a pre saved scene
    EnttScene(const std::string& file_path = "Unsaved Scene");
    /// This destructor destoyes the scene instance
    ~EnttScene();
    
    const std::string& GetName() const;

  private:
    // ------------------
    // Member variables
    // ------------------
    // Utils Memebers
    std::string file_path_ = "Unknown Path", name_ = "Unsaved Scene";    
  };
  
}
