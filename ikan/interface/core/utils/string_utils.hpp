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
    /// This function extracts the file directory from file path
    /// e.g. : "folder1/folder2/file_name.extention"
    ///         return "folder1/folder2/file_name"
    /// - Parameter path: file path for which file Extenstion need to be extracted
    static std::string GetDirectoryFromFilePath(const std::string& path);
    /// This function read the file from path given in 'file_path' and store
    /// all the date in the file in a string. Return the content of this give
    /// file as string
    /// - Parameter path: file path to be read
    static std::string ReadFromFile(const std::string& file_path);
    
    /// This function finds a word/token from string and return the char pointer
    /// starting the token
    /// - Parameters:
    ///   - str: base tring from which token need to be searched
    ///   - token: toke to be searched
    static const char*FindToken(const char* str, const std::string& token);
    /// This function finds a block of string from string and return the char
    /// pointer starting the strong 'str'
    /// - Parameters:
    ///   - str: base tring from which block need to be searched
    ///   - out_position: position of start of block as char pointer
    static std::string GetBlock(const char* str, const char** out_position);
    
    /// This function extracts the Code statement before ";"
    /// - Parameters:
    ///   - str: token to be extracted from outPosition
    ///   - outPosition: string pointer of the shader code, that will move to
    /// some offset offset is the ";" of the strcut
    static std::string GetStatement(const char* str, const char** outPosition);
    /// This function breaks the string. Break points are stored in delimiter,
    /// any character in the delimeter will be cosnider as break points
    /// - Parameters:
    ///   - string: broken strings vector
    ///   - delimiters: (break point of string). From here string will be split
    static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
    /// This function breaks the string. Break points are stored in delimiter,
    /// any character in the delimeter will be cosnider as break points
    /// - Parameters:
    ///   - string: broken strings vector
    ///   - delimiters: (break point of char). From here string will be split
    static std::vector<std::string> SplitString(const std::string& string, const char delimiter);
    /// This function breaks the string. Break points are stored in delimiter,
    /// any character in the delimeter will be cosnider as break points
    /// Split point (\t\n).
    /// - Parameter string: broken strings vector
    static std::vector<std::string> Tokenize(const std::string& string);
    /// This function breaks the string. Break points are stored in delimiter,
    /// any character in the delimeter will be cosnider as break points
    /// Split point (\n).
    /// - Parameter string: broken strings vector
    static std::vector<std::string> GetLines(const std::string& string);
  };
  
}
