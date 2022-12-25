//
//  asset_manager.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes a class to manage the asset paths

namespace ikan {
  
  class AssetManager {
  public:
    /// This function sets the Base Client Asset path. This path should be relative to .exccutable
    /// - Parameter client_asset_path: asset folder path for Client
    static void SetClientAssetPath(const std::string& client_asset_path);
    
    /// This function returns the Core Asset Base path, relative to .exce
    static const std::string& GetCoreAssetBasePath();
    /// This function returns the Client Asset Base path, relative to .exce
    static const std::string& GetClientAssetBasePath();
    
    /// This function returns the entire Asset path relative to .exce/ NOTE: Add the asset path to base path
    /// e.g.  Base path is "../../Folder1/Folder2/"
    ///     asset_path is "Folder3/File_name.Extension"
    /// returns : "../../Folder1/Folder2/Folder3/File_name.Extension"
    /// - Parameter assetPath: aasset path relative to asset folder
    static std::string CoreAsset(const std::string& asset_path);
    /// Return the entire Asset path relative to .exce/ NOTE: Add the asset path to base path
    /// e.g. Base path is "../../Folder1/Folder2/"
    ///     asset_path is "Folder3/File_name.Extension"
    /// returns : "../../Folder1/Folder2/Folder3/File_name.Extension"
    /// - Parameter assetPath: aasset path relative to asset folder
    static std::string ClientAsset(const std::string& asset_path);
    
    MAKE_PURE_STATIC(AssetManager)
    
  private:
    static std::string client_asset_path_;
    static std::string core_asset_path_;
  };
  
  using AM = AssetManager;
  
}
