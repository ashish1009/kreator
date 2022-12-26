//
//  std::string_utils.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "string_utils.hpp"

namespace ikan {
  
  /// This function returns the file name from path as std::string
  ///               last_slash  last_dot
  ///                  |               |
  /// e.g. : "folder1/folder2/file_name.extention"
  ///         return "file_name"
  /// - Parameter file_path: path of file
  std::string StringUtils::GetNameFromFilePath(const std::string& file_path) {
    auto last_slash = file_path.find_last_of("/\\");
    last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
    
    auto last_dot = file_path.find_last_of('.');
    auto count = last_dot == std::string::npos ? file_path.size() - last_slash : last_dot - last_slash;
    
    return file_path.substr(last_slash, count);
  }
  
  /// This function returns the file name from path as std::string
  ///                        last_dot     npos
  ///                          |              |
  /// e.g. : "folder1/folder2/file_name.extention"
  ///         return "file_name"
  /// - Parameter path: path of file
  std::string StringUtils::GetExtensionFromFilePath(const std::string& path) {
    auto last_dot = path.find_last_of('.');
    last_dot = last_dot == std::string::npos ? 0 : last_dot + 1;
    auto count = path.size() - last_dot;
    return path.substr(last_dot, count);
  }
  
}
