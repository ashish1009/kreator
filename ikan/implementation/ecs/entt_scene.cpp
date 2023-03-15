//
//  entt_scene.cpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#include "entt_scene.hpp"

namespace ikan {
  
  std::shared_ptr<EnttScene> EnttScene::Copy(std::shared_ptr<EnttScene> other) {
    std::shared_ptr<EnttScene> new_scene = std::make_shared<EnttScene>(other->file_path_);
    return new_scene;
  }
  
  EnttScene::EnttScene(const std::string& file_path)
  : file_path_(file_path), name_(StringUtils::GetNameFromFilePath(file_path)) {
    IK_CORE_INFO(LogModule::EnttScene, "Creating Scene ...");
    IK_CORE_INFO(LogModule::EnttScene, "  Name | {0}", name_);
  }
  
  EnttScene::~EnttScene() {
    IK_CORE_WARN(LogModule::EnttScene, "Destroying Scene!!!");
    IK_CORE_WARN(LogModule::EnttScene, "  Name | {0}", name_);
  }
  
  void EnttScene::Update(Timestep ts) {
    if (IsEditing()) {
      UpdateEditor(ts);
    }
    else {
      UpdateRuntime(ts);
    }
  }
  
  void EnttScene::UpdateEditor(Timestep ts) {
  }
  
  void EnttScene::UpdateRuntime(Timestep ts) {
  }
  
  void EnttScene::RenderGui() {
    if (IsEditing()) {
      RenderGuiEditor();
    }
    else {
      RenderGuiRuntime();
    }
  }

  void EnttScene::RenderGuiEditor() {
  }
  
  void EnttScene::RenderGuiRuntime() {
  }
  
  void EnttScene::PlayScene() {
    
  }
  
  void EnttScene::EditScene() {
    
  }
  
  void EnttScene::SetFilePath(const std::string& file_path) {
    file_path_ = file_path;
    name_ = StringUtils::GetNameFromFilePath(file_path_);
  }
  
  const std::string& EnttScene::GetName() const { return name_; }
  const std::string& EnttScene::GetFilePath() const { return file_path_; }
  bool EnttScene::IsEditing() const { return state_ == EnttScene::State::Edit; }
  
}
