//
//  scene_serializer.hpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#pragma once

#include "ecs/entt_scene.hpp"

namespace ikan {
  
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
    bool Deserialize(const std::string& file_path);
    
    DELETE_COPY_MOVE_CONSTRUCTORS(SceneSerializer);
    
  private:
    EnttScene* scene_;
  };
  
}
