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
    
    /// This function read the file from path given in 'file_path' and store
    /// all the date in the file in a string. Return the content of this give
    /// file as string
    /// - Parameter path: file path to be read
    static std::string ReadFromFile(const std::string& file_path);
  };
  
}
