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
  
  /// This function returns the file name from path as string
  ///         0           last_slash
  ///         |              |
  /// e.g. : "folder1/folder2/file_name.extention"
  ///         return "file_name"
  /// - Parameter path: path of file
  std::string StringUtils::GetDirectoryFromFilePath(const std::string& path) {
    return path.substr(0, path.find_last_of('/'));
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
      } else {
        IK_CORE_ERROR("Could not read from file '{0}'", file_path);
      }
    } else {
      IK_CORE_ERROR("Could not open file '{0}'", file_path);
    }
    return result;
  }

  const char* StringUtils::FindToken(const char* str, const std::string& token) {
    const char* t = str;
    while ((t = strstr(t, token.c_str()))) {
      // Check if left character of token is empty
      // using '/' as indicator of first character of line
      bool left = str == t or isspace(t[-1]) or t[-1] == '/';
      
      // Check if right character of token is empty
      bool right = !t[token.size()] or isspace(t[token.size()]);
      
      // Checking token is whole word
      if (left and right)
        return t;
      
      t += token.size();
    }
    return nullptr;
  }
  
  std::string StringUtils::GetBlock(const char* str, const char** out_position) {
    // get the end block brace of the string code
    const char* end = strstr(str, "}");
    
    // if not found any end bkock statement "}" then return the same string
    if (!end)
      return str;
    
    // Update the out_position pointer after the end block
    // offset the shader string to the end of current strcut
    if (out_position)
      *out_position = end;
    
    uint32_t length = (uint32_t)(end - str + 1);
    // return the substring from start to last "}"
    return std::string(str, length);
  }
  
  std::string StringUtils::GetStatement(const char* str, const char** outPosition) {
    // get the end block brace of the string code
    const char* end = strstr(str, ";");
    
    // if not found any end bkock statement ";" then return the same string
    if (!end)
      return str;
    
    // Update the out_position pointer after the end block
    // offset the shader string to the end of current strcut
    if (outPosition)
      *outPosition = end;
    
    uint32_t length = (uint32_t)(end - str + 1);
    // return the substring from start to last ";"
    return std::string(str, length);
  }
  
  std::vector<std::string> StringUtils::SplitString(const std::string& string, const std::string& delimiters) {
    size_t start = 0;
    // Find the first occurance of splitter
    size_t end = string.find_first_of(delimiters);
    
    // Stores the result
    std::vector<std::string> result;
    
    // while end of string
    while (end <= std::string::npos) {
      // Store the split block
      std::string token = string.substr(start, end - start);
      if (!token.empty())
        result.push_back(token);
      
      if (end == std::string::npos)
        break;
      
      start = end + 1;
      end = string.find_first_of(delimiters, start);
    }
    return result;
  }
  
  std::vector<std::string> StringUtils::SplitString(const std::string& string, const char delimiter) {
    return SplitString(string, std::string(1, delimiter));
  }
  
  std::vector<std::string> StringUtils::Tokenize(const std::string& string) {
    return SplitString(string, " \t\n");
  }
  
  std::vector<std::string> StringUtils::GetLines(const std::string& string) {
    return SplitString(string, "\n");
  }

}
