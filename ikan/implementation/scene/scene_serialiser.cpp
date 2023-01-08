//
//  scene_serialiser.cpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#include "scene_serialiser.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"

#include <yaml-cpp/yaml.h>

namespace ikan {
  
  SceneSerializer::SceneSerializer(EnttScene* scene) : scene_(scene) { }
  SceneSerializer::~SceneSerializer() { }

  void SceneSerializer::Serialize(const std::string& file_path) {
  }
  
  void SceneSerializer::Deserialize(const std::string& file_path) {
  }
}
