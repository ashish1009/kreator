//
//  scene.hpp
//  ikan
//
//  Created by Ashish . on 02/01/23.
//

#pragma once

// This file includes the scene class to store data of active scene

#include <entt.hpp>
#include "editor/editor_camera.hpp"

namespace ikan {

  // Forward declaration
  class Entity;

  class EnttScene {
  public:
    /// State of Scene
    enum State : uint8_t {
      Play = 0, Edit = 1
    };
    
    struct Setting {
      bool editor_camera = true;
    };

    /// This Constructor creates the instance of Scene.
    EnttScene();
    /// This destructor destoyes the scene instance
    ~EnttScene();
    
    /// This function create and Entity and store in scene registry
    /// - Parameters:
    ///   - name: name of entity
    ///   - uuid: Unique ID of entity
    [[nodiscard]] Entity CreateEntity(const std::string& name = "Unknown Entity",
                                      UUID uuid = UUID());
    /// This function destory the entity from scene registry
    /// - Parameter entity: entity to be destroyed
    void DestroyEntity(Entity entity);
    
    /// This function update the scene
    /// - Parameter ts: time step
    void Update(Timestep ts);
    /// This dunction handles the events of the scene
    /// - Parameter e: event triggered
    void EventHandler(Event& e);
    /// This function renders the GUI Window for this layer. To be called each frame from application.
    void RenderGui();

    /// This function update the viewport of the scene
    /// - Parameters:
    ///   - width: width of viewport
    ///   - height: height of viewport
    void SetViewport(uint32_t width, uint32_t height);
    
    /// This function sets the Scene as play mode
    void PlayScene();
    /// This function sets the Scene as edit mode
    void EditScene();
    
    /// This function returns the setting reference to change the setting
    Setting& GetSetting();

    // ------------------
    // Getters
    // ------------------
    /// This function returns the state of scene
    State GetState() const;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EnttScene);
    
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
    
    /// This function handles the event of the scene in edit mode
    /// - Parameter event event triggered
    void EventHandlerEditor(Event& event);
    /// This function handles the event of the scene in play mode
    /// - Parameter event event triggered
    void EventHandlerRuntime(Event& event);
    
    /// This function renderes the imgui of the scene in edit mode
    void RenderImguiEditor();
    /// This function renderes the imgui of the scene in play mode
    void RenderImguiRuntime();

    // ------------------
    // Member variabls
    // ------------------
    // Registry to store the entity handles
    entt::registry registry_;
    std::unordered_map<entt::entity, Entity> entity_id_map_;

    // Number of Entity stored in Scene and Max ID given to Entity
    uint32_t num_entities_ = 0, max_entity_id_ = -1;

    EditorCamera editor_camera_;
    
    // State of the scene
    State state_ = State::Edit;

    Setting setting_;
    
    // Function pointers
    std::function<void(Timestep)> update_;
    std::function<void(Event&)> event_handler_;
    std::function<void()> render_imgui_;

    friend class Entity;
    friend class ScenePanelManager;
  };
  
}
