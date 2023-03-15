//
//  entt_scene.cpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#include "entt_scene.hpp"

namespace ikan {
  
  EnttScene::EnttScene(const std::string& file_path)
  : file_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)) {
    IK_CORE_INFO(LogModule::EnttScene, "Creating Scene ...");
    IK_CORE_INFO(LogModule::EnttScene, "  Name | {0}", name_);
  }
  
  EnttScene::~EnttScene() {
    IK_CORE_WARN(LogModule::EnttScene, "Destroying Scene!!!");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", name_);
  }
  
  const std::string& EnttScene::GetName() const { return name_; }
}
