//
//  asset_manager.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "asset_manager.hpp"

namespace ikan {
  
  std::string AssetManager::client_asset_path_;
  std::string AssetManager::core_asset_path_ = "../../../ikan/core_assets/";
  
  const std::string& AssetManager::GetCoreAssetBasePath() {
    return core_asset_path_;
  }
  const std::string& AssetManager::GetClientAssetBasePath() {
    return client_asset_path_;
  }
  
  std::string AssetManager::CoreAsset(const std::string& asset_path) {
    return core_asset_path_ + asset_path;
  }
  std::string AssetManager::ClientAsset(const std::string& asset_path) {
    return client_asset_path_ + asset_path;
  }
    
  void AssetManager::SetClientAssetPath(const std::string& client_asset_path) {
    client_asset_path_ = client_asset_path;
    IK_CORE_INFO("Client Asset path is set to {0}", client_asset_path_);
  }
  
}
