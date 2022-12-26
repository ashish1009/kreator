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
  
  std::string StringUtils::ReadFromFile(const std::string& file_path) {
    std::string result = "";
    std::ifstream in(file_path, std::ios::in | std::ios::binary);
    if (in) {
      in.seekg(0, std::ios::end);
      size_t size = (size_t)in.tellg();
      if (-1 != (int32_t)size) {
        result.resize(size);
        in.seekg(0, std::ios::beg);
        in.read(&result[0], (long)size);
        in.close();
      }
      else {
        IK_CORE_ERROR("Could not read from file '{0}'", file_path);
      }
    }
    else {
      IK_CORE_ERROR("Could not open file '{0}'", file_path);
    }
    return result;
  }

  
}
