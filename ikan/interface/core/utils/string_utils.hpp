//
//  string_utils.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the string utils API

namespace ikan {
  
  class StringUtils {
  public:
    /// This function returns the file name from path as string
    /// e.g. : "folder1/folder2/file_name.extention"
    ///         return "file_name"
    /// - Parameter file_path: path of file
    static std::string GetNameFromFilePath(const std::string& file_path);
    /// This function extracts the file extension from file path
    /// e.g. : "folder1/folder2/file_name.extention"
    ///         return "extension"
    /// - Parameter path: file path for which file Extenstion need to be extracted
    static std::string GetExtensionFromFilePath(const std::string& path);
  };
  
}
