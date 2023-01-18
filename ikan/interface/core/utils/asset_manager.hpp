//
//  asset_manager.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

namespace ikan {
  
  /// This class stores all the required directorys
  /// - Core Asset path
  /// - Client Asset path
  /// - Project path and so on...
  class DirectoryManager {
  public:
    /// This function sets the Base Client Asset path:
    /// - Parameter client_asset_path: Asset folder path for Client:
    /// - Important: This path should be relative to .exccutable
    static void SetClientAssetPath(const std::string& client_asset_path);
    
    /// This function returns the Core Asset Base path:
    /// - Important: Returned path is relative to .exce and upto the folder name of 'Assets'
    static const std::string& GetCoreAssetBasePath();
    /// This function returns the Client Asset Base path
    /// - Important: Returned path is relative to .exce and upto the folder name of 'Assets'
    static const std::string& GetClientAssetBasePath();
    /// This function returns the Workspace
    /// - Important: Returned path is relative to .exce and upto the folder name of 'Assets'
    static const std::string& GetWorkspaceBasePath();

    /// This function returns the Workspace path relative to workspace
    /// - Parameter asset_path: Aasset path relative to workspace folder:
    /// - Important: Add the asset path to base:
    ///               e.g.  Base path is "../../../" :
    ///               asset_path is "Folder3/File_name.Extension" :
    ///               returns - "../../../Folder2/Folder3/File_name.Extension"
    static std::string ProjectPath(const std::string& asset_path);
    /// This function returns the path relative to core Asset path
    /// - Parameter asset_path: Aasset path relative to asset folder:
    /// - Important: Add the asset path to base:
    ///               e.g.  Base path is "../../Folder1/Folder2/" :
    ///               asset_path is "Folder3/File_name.Extension" :
    ///               returns - "../../Folder1/Folder2/Folder3/File_name.Extension"
    static std::string CoreAsset(const std::string& asset_path);
    /// This function returns the path relative to client Asset path
    /// - Parameter asset_path: Aasset path relative to asset folder:
    /// - Important: Add the asset path to base:
    ///               e.g.  Base path is "../../Folder1/Folder2/" :
    ///               asset_path is "Folder3/File_name.Extension" :
    ///               returns - "../../Folder1/Folder2/Folder3/File_name.Extension"
    static std::string ClientAsset(const std::string& asset_path);
    
    MAKE_PURE_STATIC(DirectoryManager)
    
  private:
    static std::string client_asset_path_;
    static std::string core_asset_path_;
    static std::string project_path_;
  };
  
  using AM = DirectoryManager;
  
}
