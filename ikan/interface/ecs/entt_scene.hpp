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
#include "ecs/scene_camera.hpp"

#include "box2d/b2_world.h"

namespace ecs {
  
  using namespace ikan;

  // Forward declaration
  class Entity;
  // TODO: For debug only
  struct TransformComponent;
  
  struct CameraData {
    SceneCamera* scene_camera = nullptr;
    glm::vec3 position;
    glm::mat4 transform_matrix;
    
    // TODO: For debugging only
    TransformComponent* transform_comp;
    
    CameraData() = default;
    DELETE_COPY_MOVE_CONSTRUCTORS(CameraData);
  };

  class EnttScene {
  public:
    /// State of Scene
    enum State : uint8_t {
      Play = 0, Edit = 1
    };
    
    struct Setting {
      bool editor_camera = true;
      bool scene_controller = true;
      bool scene_data = true;
    };

    /// This Constructor creates the instance of Scene.
    /// - Parameters:
    ///   - file_path: optional file path if we want to create a pre saved scene
    EnttScene(const std::string& file_path = "Unsaved Scene");
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
    
    /// This function update the scene path
    /// - Parameter file_path: file path
    void SetFilePath(const std::string& file_path);
    
    // ------------------
    // Getters
    // ------------------
    /// This function returns the setting reference to change the setting
    Setting& GetSetting();
    /// This function returns the entity Ref from its id
    /// - Parameter id: entity ID
    Entity* GetEnitityFromId(int32_t id);
    /// This function returns the state of scene
    State GetState() const;
    /// This function returns the number of Entities stored in Scene
    uint32_t GetNumEntities() const;
    /// This function returns the Max Entity ID given to scene
    uint32_t GetMaxEntityId() const;
    /// This function returns the editor camera of scene
    EditorCamera* GetEditorCamera();
    /// This function returns the reference of registry
    entt::registry& GetRegistry();
    /// This finction return is scene is in edit state
    bool IsEditing() const;
    /// This finction return Name of scene
    const std::string GetName() const;
    /// This function returns the primary camera data
    const CameraData& GetPrimaryCameraData() const;
    /// This function returns the flag to use editor camera
    bool UseEditorCamera() const;
    
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
    
    /// This function updates the primary camera data
    void UpdatePrimaryCameraData();
    /// This functuion Insantiates all the native script
    /// - Parameter ts: time step of each frame
    void InstantiateScript(Timestep ts);

    /// This function renders the 2D Entities
    /// - Parameter camera_view_projection_mat: camera view projection matrix
    void Render2DEntities(const glm::mat4& camera_view_projection_mat);
    /// This function renders the 2D Entities
    void RenderBoudningBox();
    
    /// This function starts the runtime physics
    void RuntimeStart();

    // ------------------
    // Member variabls
    // ------------------
    // Registry to store the entity handles
    entt::registry registry_;
    std::unordered_map<entt::entity, Entity> entity_id_map_;

    // Number of Entity stored in Scene and Max ID given to Entity
    uint32_t num_entities_ = 0, max_entity_id_ = -1;

    // Utils Memebers
    std::string file_path_ = "Unknown Path", name_ = "Unsaved Scene";

    // Editor camera
    EditorCamera editor_camera_;
    
    // Viewport data
    uint32_t viewport_width_ = 0, viewport_height_ = 0;
    
    // State of the scene
    State state_ = State::Edit;

    // Scene primary camera
    CameraData primary_camera_data_;
    
    // Function pointers
    std::function<void(Timestep)> update_;
    std::function<void(Event&)> event_handler_;
    std::function<void()> render_imgui_;
    
    // Scene Debugger
    Setting setting_;
    bool use_editor_camera_ = true;
    
    // Physics Data
    b2World* physics_world_ = nullptr;
    
    friend class Entity;
    friend class ScenePanelManager;
    friend class SceneSerializer;
  };
  
}
