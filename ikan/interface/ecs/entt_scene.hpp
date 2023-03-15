//
//  entt_scene.hpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#pragma once

#include <entt.hpp>
#include "core/utils/time_step.hpp"

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
    
    /// This function update the scene
    /// - Parameter ts: time step
    void Update(Timestep ts);
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    void RenderGui();

    /// This function sets the Scene as play mode
    void PlayScene();
    /// This function sets the Scene as edit mode
    void EditScene();
    
    /// This function update the scene path
    /// - Parameter file_path: file path
    void SetFilePath(const std::string& file_path);

    /// This function returns the name of scene
    const std::string& GetName() const;
    /// This finction return File path of scene
    const std::string& GetFilePath() const;
    /// This finction return is scene is in edit state
    bool IsEditing() const;

    /// This function create new scene copy the scene data from argument
    /// - Parameter other: copy scene
    static std::shared_ptr<EnttScene> Copy(std::shared_ptr<EnttScene> other);

  private:
    // ------------------
    // Member functions
    // ------------------
    /// This function updates the scene in edit mode
    /// - Parameter ts time step
    void UpdateEditor(Timestep ts);
    /// This function updates the scene in play
    /// - Parameter ts time step
    void UpdateRuntime(Timestep ts);

    /// This function renderes the imgui of the scene in edit mode
    void RenderGuiEditor();
    /// This function renderes the imgui of the scene in play mode
    void RenderGuiRuntime();

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
