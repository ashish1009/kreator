//
//  scene_serializer.cpp
//  ikan
//
//  Created by Ashish . on 15/03/23.
//

#include "scene_serializer.hpp"
#include "entt_scene.hpp"

#include <yaml-cpp/yaml.h>

namespace YAML {
  // yml converstions vec2
  template<> struct convert<glm::vec2> {
    static Node encode(const glm::vec2& rhs) {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.SetStyle(EmitterStyle::Flow);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec2& rhs) {
      if (!node.IsSequence() or node.size() != 2)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      return true;
    }
  };
  
  // yml converstions vec3
  template<> struct convert<glm::vec3> {
    static Node encode(const glm::vec3& rhs) {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.SetStyle(EmitterStyle::Flow);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec3& rhs) {
      if (!node.IsSequence() or node.size() != 3)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      return true;
    }
  };
  
  // yml converstions
  template<> struct convert<glm::vec4> {
    static Node encode(const glm::vec4& rhs) {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      node.push_back(rhs.w);
      node.SetStyle(EmitterStyle::Flow);
      return node;
    }
    
    static bool decode(const Node& node, glm::vec4& rhs) {
      if (!node.IsSequence() or node.size() != 4)
        return false;
      
      rhs.x = node[0].as<float>();
      rhs.y = node[1].as<float>();
      rhs.z = node[2].as<float>();
      rhs.w = node[3].as<float>();
      return true;
    }
  };
  
} // namespace YAML

namespace ikan {
  
  // yml << operator for glm vec 3
  static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
  }
  // yml << operator for glm vec 2
  static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
  }
  // yml << operator for glm vec 4
  static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.a << YAML::EndSeq;
    return out;
  }
      
  SceneSerializer::SceneSerializer(EnttScene* scene) : scene_(scene) { }
  SceneSerializer::~SceneSerializer() { }
  
  void SceneSerializer::Serialize(const std::string& file_path) {
    IK_CORE_INFO(LogModule::SceneSerializer, "Serialising a Scene");
    IK_CORE_INFO(LogModule::SceneSerializer, "  Path | {0}", file_path);
    IK_CORE_INFO(LogModule::SceneSerializer, "  Name | {0}", scene_->name_);
    
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    

    std::ofstream fout(file_path);
    fout << out.c_str();
  }
  
  bool SceneSerializer::Deserialize(const std::string& file_path) {
    if (file_path == "") return false;
    
    YAML::Node data = YAML::LoadFile(file_path);
    
    if (!data["Scene"])
      return false;
    
    std::string scene_name = data["Scene"].as<std::string>();
    IK_CORE_INFO(LogModule::SceneSerializer, "Deserialising scene");
    IK_CORE_INFO(LogModule::SceneSerializer, "  Path | {0}", file_path);
    IK_CORE_INFO(LogModule::SceneSerializer, "  Name | {0}", scene_name);
    
    return true;
  }
  
}
