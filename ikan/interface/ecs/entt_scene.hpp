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
    /// State of Scene
    enum State : uint8_t {
      Play = 0, Edit = 1
    };

    /// This Constructor creates the instance of Scene.
    /// - Parameter file_path: optional file path if we want to create a pre saved scene
    EnttScene(const std::string& file_path = "Unsaved Scene");
    /// This destructor destoyes the scene instance
    ~EnttScene();
    
    /// This function sets the Scene as play mode
    void PlayScene();
    /// This function sets the Scene as edit mode
    void EditScene();
    
    /// This function returns the name of scene
    const std::string& GetName() const;
    /// This finction return File path of scene
    const std::string& GetFilePath() const;
    /// This finction return is scene is in edit state
    bool IsEditing() const;

  private:
    // ------------------
    // Member variables
    // ------------------
    // Utils Memebers
    std::string file_path_ = "Unknown Path", name_ = "Unsaved Scene";

    // State of the scene
    State state_ = State::Edit;

    friend class SceneSerializer;
  };
  
}
