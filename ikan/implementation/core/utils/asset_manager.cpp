//
//  asset_manager.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "asset_manager.hpp"

namespace ikan {
  
  std::string DirectoryManager::client_asset_path_;
  std::string DirectoryManager::core_asset_path_ = "../../../ikan/core_assets/";
  std::string DirectoryManager::project_path_ = "../../../";

  const std::string& DirectoryManager::GetCoreAssetBasePath() {
    return core_asset_path_;
  }
  const std::string& DirectoryManager::GetClientAssetBasePath() {
    return client_asset_path_;
  }
  const std::string& DirectoryManager::GetWorkspaceBasePath() {
    return project_path_;
  }
  
  std::string DirectoryManager::ProjectPath(const std::string &asset_path) {
    return project_path_ + asset_path;
  }
  std::string DirectoryManager::CoreAsset(const std::string& asset_path) {
    return core_asset_path_ + asset_path;
  }
  std::string DirectoryManager::ClientAsset(const std::string& asset_path) {
    return client_asset_path_ + asset_path;
  }
    
  void DirectoryManager::SetClientAssetPath(const std::string& client_asset_path) {
    client_asset_path_ = client_asset_path;
    IK_CORE_TRACE(LogModule::AssetManager, "Client Asset path is set to {0}", client_asset_path_);
  }
  
}
