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
  
  void EnttScene::PlayScene() {
    
  }
  
  void EnttScene::EditScene() {
    
  }
  
  const std::string& EnttScene::GetName() const { return name_; }
  const std::string& EnttScene::GetFilePath() const { return file_path_; }
  bool EnttScene::IsEditing() const { return state_ == EnttScene::State::Edit; }
  
}
