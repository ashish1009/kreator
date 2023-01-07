//
//  core_components.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

#include "renderer/graphics/texture.hpp"
#include "scene/scene_camera.hpp"
#include "scene/scriptable_entity.hpp"

namespace ikan {
 
  struct IDComponent {
    UUID id = 0;
    IDComponent(const UUID& id);
    DEFINE_COPY_MOVE_CONSTRUCTORS(IDComponent);
  };

  struct TagComponent {
    std::string tag = "Default Entity";
    TagComponent(const std::string& tag);
    DEFINE_COPY_MOVE_CONSTRUCTORS(TagComponent);
  };
  
  struct TransformComponent {
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    
    TransformComponent(const glm::vec3& translation = { 0.0f, 0.0f, 0.0f });
    glm::mat4 GetTransform() const;
    void RenderGui();

    DEFINE_COPY_MOVE_CONSTRUCTORS(TransformComponent);
  };
  
  struct QuadComponent {
    TextureComponent texture_comp;
    glm::vec4 color{1.0f};

    void RenderGui();

    QuadComponent();
    DEFINE_COPY_MOVE_CONSTRUCTORS(QuadComponent);
  };
  
  struct SpriteComponent {
    std::shared_ptr<SubTexture> sub_texture;
    SpriteComponent(std::shared_ptr<SubTexture> sub_tex = nullptr);
    
    void RenderGui();

    DEFINE_COPY_MOVE_CONSTRUCTORS(SpriteComponent);
  };

  struct CircleComponent {
    TextureComponent texture_comp;
    glm::vec4 color{1.0f};

    float thickness = 1.0f;
    float fade = 0.005f;
    
    void RenderGui();

    CircleComponent();
    DEFINE_COPY_MOVE_CONSTRUCTORS(CircleComponent);
  };
  
  struct CameraComponent {
    bool is_fixed_aspect_ratio = false;
    bool is_primary = false;
    std::shared_ptr<SceneCamera> camera;
    
    void RenderGui();
    
    CameraComponent(SceneCamera::ProjectionType proj_type = SceneCamera::ProjectionType::Orthographic);
    DEFINE_COPY_MOVE_CONSTRUCTORS(CameraComponent);
  };

  struct NativeScriptComponent {
    std::string script_name;
    uint32_t script_idx = 0;
    ScriptableEntity* instance = nullptr;
    
    ScriptableEntity*(*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent*);
    
    template<typename T>
    void Bind() {
      InstantiateScript = []() {
        return static_cast<ScriptableEntity*>(new T());
      };
      
      DestroyScript = [](NativeScriptComponent* nsc) {
        delete nsc->instance;
        nsc->instance = nullptr;
      };
      
      // Store the script name
      int32_t status;
      std::string tname = typeid(T).name();
      char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
      script_name = demangled_name;
      
      // Delete the allocated memory
      if(status == 0) {
        tname = demangled_name;
        std::free(demangled_name);
      }
    }
    
    void RenderGui();
    
    NativeScriptComponent() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(NativeScriptComponent);
  };
  
  struct RigidBodyComponent {
    void RenderGui();
  };

}
