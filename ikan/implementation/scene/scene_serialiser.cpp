//
//  scene_serialiser.cpp
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

#include "scene_serialiser.hpp"
#include "scene/entity.hpp"
#include "scene/core_components.hpp"

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
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    // Serialize the scene for all the entities
    for (const auto& [entt_id, entity] : scene_->entity_id_map_) {
      out << YAML::BeginMap; // Entity
      out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

      // ------------------------------------------------------------------------
      if (entity.HasComponent<TagComponent>()) {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent
        
        auto& tag = entity.GetComponent<TagComponent>().tag;
        out << YAML::Key << "Tag" << YAML::Value << tag;
        
        out << YAML::EndMap; // TagComponent
      }
      
      // ------------------------------------------------------------------------
      if (entity.HasComponent<TransformComponent>()) {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent
        
        auto& tc = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << tc.translation;
        out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.scale;
        
        out << YAML::EndMap; // TransformComponent
      }
      
      // ------------------------------------------------------------------------
      if (entity.HasComponent<CameraComponent>()) {
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap; // CameraComponent
        
        auto& cc = entity.GetComponent<CameraComponent>();
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.is_fixed_aspect_ratio;
        out << YAML::Key << "Primary" << YAML::Value << cc.is_primary;
        
        // Scene Camera
        const auto& camera = cc.camera;
        out << YAML::Key << "ProjectionType" << YAML::Value << (uint32_t)camera->GetProjectionType();
        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera->GetPerspectiveFOV();
        out << YAML::Key << "OrthographicSize" << YAML::Value << camera->GetOrthographicSize();
        out << YAML::Key << "Near" << YAML::Value << camera->GetNear();
        out << YAML::Key << "Far" << YAML::Value << camera->GetFar();
        
        out << YAML::EndMap; // CameraComponent
      }
      
      // ------------------------------------------------------------------------
      if (entity.HasComponent<QuadComponent>()) {
        out << YAML::Key << "QuadComponent";
        out << YAML::BeginMap; // QuadComponent
        
        auto& qc = entity.GetComponent<QuadComponent>();
        out << YAML::Key << "Texture_Use" << YAML::Value << qc.texture_comp.use;

        if (qc.texture_comp.component)
          out << YAML::Key << "Texture_Path" << YAML::Value << qc.texture_comp.component->GetfilePath();
        else
          out << YAML::Key << "Texture_Path" << YAML::Value << "";

        out << YAML::Key << "Texture_TilingFactor" << YAML::Value << qc.texture_comp.tiling_factor;

        out << YAML::Key << "Color" << YAML::Value << qc.color;
        
        out << YAML::EndMap; // QuadComponent
      }
      
      // ------------------------------------------------------------------------
      if (entity.HasComponent<CircleComponent>()) {
        out << YAML::Key << "CircleComponent";
        out << YAML::BeginMap; // CircleComponent
        
        auto& cc = entity.GetComponent<CircleComponent>();
        out << YAML::Key << "Texture_Use" << YAML::Value << cc.texture_comp.use;
        
        if (cc.texture_comp.component)
          out << YAML::Key << "Texture_Path" << YAML::Value << cc.texture_comp.component->GetfilePath();
        else
          out << YAML::Key << "Texture_Path" << YAML::Value << "";
        
        out << YAML::Key << "Texture_TilingFactor" << YAML::Value << cc.texture_comp.tiling_factor;
        
        out << YAML::Key << "Color" << YAML::Value << cc.color;

        out << YAML::Key << "Thickness" << YAML::Value << cc.thickness;
        out << YAML::Key << "Fade" << YAML::Value << cc.fade;

        out << YAML::EndMap; // CircleComponent
      }

      // ------------------------------------------------------------------------
      if (entity.HasComponent<NativeScriptComponent>()) {
        out << YAML::Key << "NativeScriptComponent";
        out << YAML::BeginMap; // NativeScriptComponent
        
        auto& sc = entity.GetComponent<NativeScriptComponent>();
        std::string name_tag = "Script_name_";
        int32_t i = 0;
        for (const auto& [name, scriot] : sc.scrip_name_map) {
          name_tag += std::to_string(i++);
          out << YAML::Key << name_tag << YAML::Value << name;
        }
        
        out << YAML::Key << "Num_Scripts" << YAML::Value << i;

        out << YAML::EndMap; // NativeScriptComponent
      }

      out << YAML::EndMap; // Entity
    } // for (const auto& [uuid, entity] : scene_->entity_id_map_)
    
    out << YAML::EndSeq;
    out << YAML::EndMap;
    
    std::ofstream fout(file_path);
    fout << out.c_str();

  }
  
  void SceneSerializer::Deserialize(const std::string& file_path) {
    YAML::Node data = YAML::LoadFile(file_path);
    
    if (!data["Scene"])
      return;
        
    std::string scene_name = data["Scene"].as<std::string>();
    IK_CORE_INFO(LogModule::SceneSerializer, "Deserialising scene");
    IK_CORE_INFO(LogModule::SceneSerializer, "  Path | {0}", file_path);
    IK_CORE_INFO(LogModule::SceneSerializer, "  Name | {0}", scene_name);

    auto entities = data["Entities"];
    if (entities) {
      for (auto entity : entities) {
        uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO
        
        std::string name, group;
        auto tag_component = entity["TagComponent"];
        if (tag_component) {
          name = tag_component["Tag"].as<std::string>();
        }
        
        Entity deserialized_entity = scene_->CreateEntity(name, uuid);
        IK_CORE_INFO(LogModule::SceneSerializer, "  Deserialising Entity");
        IK_CORE_INFO(LogModule::SceneSerializer, "  ID    | {0}", uuid);
        IK_CORE_INFO(LogModule::SceneSerializer, "  Name  | {0}", name);
                
        // --------------------------------------------------------------------
        auto transform_component = entity["TransformComponent"];
        if (transform_component) {
          // Entities always have transforms
          auto& tc = deserialized_entity.GetComponent<TransformComponent>();
          tc.translation = transform_component["Translation"].as<glm::vec3>();
          tc.rotation = transform_component["Rotation"].as<glm::vec3>();
          tc.scale = transform_component["Scale"].as<glm::vec3>();
          
          IK_CORE_INFO(LogModule::SceneSerializer, "    Transform Component");
          IK_CORE_INFO(LogModule::SceneSerializer, "      Translation   | {0} | {1} | {2}", tc.translation.x, tc.translation.y, tc.translation.z);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Rotation      | {0} | {1} | {2}", tc.rotation.x, tc.rotation.y, tc.rotation.z);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Scale         | {0} | {1} | {2}", tc.scale.x, tc.scale.y, tc.scale.z);
        } // if (transform_component)
        
        // --------------------------------------------------------------------
        auto camera_component = entity["CameraComponent"];
        if (camera_component) {
          auto type = camera_component["ProjectionType"].as<uint32_t>();
          
          auto& cc = deserialized_entity.AddComponent<CameraComponent>((SceneCamera::ProjectionType)type);
          cc.is_fixed_aspect_ratio = camera_component["FixedAspectRatio"].as<bool>();
          cc.is_primary = camera_component["Primary"].as<bool>();
          
          auto fov = camera_component["PerspectiveFOV"].as<float>();
          auto size = camera_component["OrthographicSize"].as<float>();
          auto near = camera_component["Near"].as<float>();
          auto far = camera_component["Far"].as<float>();
                    
          if ((SceneCamera::ProjectionType)type == SceneCamera::ProjectionType::Orthographic)
            cc.camera->SetOrthographic(size, near, far);
          else if ((SceneCamera::ProjectionType)type == SceneCamera::ProjectionType::Perspective)
            cc.camera->SetPerspective(fov, near, far);
          
          IK_CORE_INFO(LogModule::SceneSerializer, "    Cameara Component");
          IK_CORE_INFO(LogModule::SceneSerializer, "      Primary             | {0}", cc.is_primary);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Fixed Aspect Ratio  | {0}", cc.is_fixed_aspect_ratio);
          
          if ((SceneCamera::ProjectionType)type == SceneCamera::ProjectionType::Orthographic) {
            IK_CORE_INFO(LogModule::SceneSerializer, "      Orthographic Camera");
            IK_CORE_INFO(LogModule::SceneSerializer, "        Size | {0}", cc.camera->GetOrthographicSize());
          }
          else if ((SceneCamera::ProjectionType)type == SceneCamera::ProjectionType::Perspective) {
            IK_CORE_INFO(LogModule::SceneSerializer, "      Perspective Camera");
            IK_CORE_INFO(LogModule::SceneSerializer, "        FOV  | {0}", cc.camera->GetPerspectiveFOV());
          }
          
          IK_CORE_INFO(LogModule::SceneSerializer, "         Near : {0}", cc.camera->GetNear());
          IK_CORE_INFO(LogModule::SceneSerializer, "         Far  : {0}", cc.camera->GetFar());
        } // if (camera_component)
        
        // --------------------------------------------------------------------
        auto quad_component = entity["QuadComponent"];
        if (quad_component) {
          auto& qc = deserialized_entity.AddComponent<QuadComponent>();

          qc.texture_comp.use = quad_component["Texture_Use"].as<bool>();
          
          std::string texture_path = quad_component["Texture_Path"].as<std::string>();
          if (texture_path != "")
            qc.texture_comp.component = Renderer::GetTexture(texture_path);

          qc.texture_comp.tiling_factor = quad_component["Texture_TilingFactor"].as<float>();
          qc.color = quad_component["Color"].as<glm::vec4>();
          
          IK_CORE_INFO(LogModule::SceneSerializer, "    Quad Component");
          IK_CORE_INFO(LogModule::SceneSerializer, "      Texture");
          IK_CORE_INFO(LogModule::SceneSerializer, "        Use             | {0}", qc.texture_comp.use);
          if (qc.texture_comp.component)
            IK_CORE_INFO(LogModule::SceneSerializer, "        Path            | {0}", qc.texture_comp.component->GetfilePath());
          else
            IK_CORE_INFO(LogModule::SceneSerializer, "        No Texture      ");
          IK_CORE_INFO(LogModule::SceneSerializer, "        Tiling Factor   | {0}", qc.texture_comp.tiling_factor);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Color | {0} | {1} | {2}", qc.color.x, qc.color.y, qc.color.z);

        } // if (quad_component)

        // --------------------------------------------------------------------
        auto circle_component = entity["CircleComponent"];
        if (circle_component) {
          auto& cc = deserialized_entity.AddComponent<CircleComponent>();
          
          cc.texture_comp.use = circle_component["Texture_Use"].as<bool>();
          
          std::string texture_path = circle_component["Texture_Path"].as<std::string>();
          if (texture_path != "")
            cc.texture_comp.component = Renderer::GetTexture(texture_path);
          
          cc.texture_comp.tiling_factor = circle_component["Texture_TilingFactor"].as<float>();
          
          cc.color = circle_component["Color"].as<glm::vec4>();
          cc.thickness = circle_component["Thickness"].as<float>();
          cc.fade = circle_component["Fade"].as<float>();
          
          IK_CORE_INFO(LogModule::SceneSerializer, "    Circle Component");
          IK_CORE_INFO(LogModule::SceneSerializer, "      Texture");
          IK_CORE_INFO(LogModule::SceneSerializer, "        Use             | {0}", cc.texture_comp.use);
          if (cc.texture_comp.component)
            IK_CORE_INFO(LogModule::SceneSerializer, "        Path            | {0}", cc.texture_comp.component->GetfilePath());
          else
            IK_CORE_INFO(LogModule::SceneSerializer, "        No Texture      ");
          IK_CORE_INFO(LogModule::SceneSerializer, "        Tiling Factor   | {0}", cc.texture_comp.tiling_factor);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Color | {0} | {1} | {2}", cc.color.x, cc.color.y, cc.color.z);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Thickness         | {0}", cc.thickness);
          IK_CORE_INFO(LogModule::SceneSerializer, "      Fade              | {0}", cc.fade);

        } // if (circle_component)
        
        // --------------------------------------------------------------------
        auto script_component = entity["NativeScriptComponent"];
        if (script_component) {
          auto& sc = deserialized_entity.AddComponent<NativeScriptComponent>();
          int32_t num_scripts = script_component["Num_Scripts"].as<int32_t>();
          std::string name_tag = "Script_name_";
          for (int i = 0; i < num_scripts; i++) {
            name_tag += std::to_string(i);
            std::string script_name = script_component[name_tag].as<std::string>();
            
            ScriptManager::UpdateScript(&sc, script_name, nullptr);
          }

          IK_CORE_INFO(LogModule::SceneSerializer, "    Script Component");
        } // if (script_component)

      } // for (auto entity : entities)
    } // if (entities)
  }
  
}