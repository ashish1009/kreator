//
//  scene_serialiser.hpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#pragma once

// This file includes the ScenSerializer class to load and save the scene

#include "ecs/entt_scene.hpp"

namespace ecs {
  
  using namespace ikan;
  
  class SceneSerializer {
  public:
    /// This is Constructor creates instance of Scene serialize and store the scene as pointer
    /// - Parameter scene: scene pointer
    SceneSerializer(EnttScene* scene);
    /// This destructor destroyes Scene Serializer Destructor
    ~SceneSerializer();
    
    /// This functions serializes(Saves) the scene at path 'file_path'
    /// - Parameter file_path: path wehre scene need to be saved
    void Serialize(const std::string& file_path);
    /// This functions serializes(Opens) the scene frol path 'file_path'
    /// - Parameter file_path: path which need to be loaded
    void Deserialize(const std::string& file_path);
    
    DELETE_COPY_MOVE_CONSTRUCTORS(SceneSerializer);
    
  private:
    EnttScene* scene_;
  };
  
}
