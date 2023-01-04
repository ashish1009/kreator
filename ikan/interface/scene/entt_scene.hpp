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
    /// This Constructor creates the instance of Scene.
    EnttScene();
    /// This destructor destoyes the scene instance
    ~EnttScene();
    
    /// This function create and Entity and store in scene registry
    /// - Parameters:
    ///   - name: name of entity
    ///   - uuid: Unique ID of entity
    Entity CreateEntity(const std::string& name = "Unknown Entity",
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
    
    DELETE_COPY_MOVE_CONSTRUCTORS(EnttScene);
    
  private:
    // ------------------
    // Member variabls
    // ------------------
    // Registry to store the entity handles
    entt::registry registry_;
    std::unordered_map<entt::entity, Entity> entity_id_map_;

    // Number of Entity stored in Scene and Max ID given to Entity
    uint32_t num_entities_ = 0, max_entity_id_ = -1;

    EditorCamera editor_camera_;

    friend class Entity;
  };
  
}
