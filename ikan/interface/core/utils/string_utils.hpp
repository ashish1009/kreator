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
    /// - Parameter file_path: Path of file for which file Name need to be extracted:
    /// - Important: e.g. : "folder1/folder2/file_name.extention" return "file_name"
    static std::string GetNameFromFilePath(const std::string& file_path);
    /// This function returns the file extention from path as string
    /// - Parameter file_path File path for which file Extenstion need to be extracted:
    /// - Important: e.g. : "folder1/folder2/file_name.extention" return "extension"
    static std::string GetExtensionFromFilePath(const std::string& file_path);
    /// This function returns the Directory from path as string
    /// - Parameter file_path: File path for which directory need to be extracted:
    /// - Important: e.g. :  "folder1/folder2/file_name.extention" return "folder1/folder2/file_name"
    static std::string GetDirectoryFromFilePath(const std::string& file_path);
    /// This function read the file from path given in 'file_path' and store all the date in the file in a string.
    /// Return the content of this give file as string
    /// - Parameter file_path: File path to be read:
    static std::string ReadFromFile(const std::string& file_path);
    
    /// This function finds a word/token from string and return the char pointer starting the token
    /// - Parameters:
    ///   - str: Base string from which token need to be searched:
    ///   - token: Token to be searched:
    static const char*FindToken(const char* str, const std::string& token);
    /// This function finds a block of string from string and return the char pointer starting the strong 'str'
    /// - Parameters:
    ///   - str: Base string from which Block start pointer is returned:
    ///   - out_position: Output position pointer of block:
    /// - Important: Returns the pointer at the start of any block. Block start is considered as '{'
    static std::string GetBlock(const char* str, const char** out_position);
    
    /// This function extracts the Code statement before ";"
    /// - Parameters:
    ///   - str: token to be extracted from outPosition
    ///   - out_position: string pointer of the shader code, that will move to some offset offset is the ";" of the strcut
    static std::string GetStatement(const char* str, const char** out_position);
    /// This function breaks the string. Break points are stored in delimiter, any character in the
    /// delimeter will be cosnider as break points
    /// - Parameters:
    ///   - string: broken strings vector
    ///   - delimiters: (break point of string). From here string will be split
    static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
    /// This function breaks the string. Break points are stored in delimiter, any character in the delimeter
    /// will be cosnider as break points
    /// - Parameters:
    ///   - string: broken strings vector
    ///   - delimiters: (break point of char). From here string will be split
    static std::vector<std::string> SplitString(const std::string& string, const char delimiter);
    /// This function breaks the string. Break points are stored in delimiter, any character in the delimeter
    /// will be cosnider as break points Split point (\t\n).
    /// - Parameter string: broken strings vector
    static std::vector<std::string> Tokenize(const std::string& string);
    /// This function breaks the string. Break points are stored in delimiter, any character in the delimeter
    /// will be cosnider as break points Split point (\n).
    /// - Parameter string: broken strings vector
    static std::vector<std::string> GetLines(const std::string& string);
  };
  
}
