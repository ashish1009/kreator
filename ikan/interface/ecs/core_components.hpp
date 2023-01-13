//
//  core_components.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

#include "renderer/graphics/texture.hpp"
#include "ecs/scene_camera.hpp"
#include "ecs/scriptable_entity.hpp"
#include "physics/aabb.hpp"

namespace ecs {
  
  using namespace ikan;
  
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
    TransformComponent(const glm::vec3& translation = { 0.0f, 0.0f, 0.0f });
    void RenderGui();

    glm::mat4 transform;
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    const glm::mat4& GetTransform() const;
    const glm::vec3& Translation() const;
    const glm::vec3& Rotation() const;
    const glm::vec3& Scale() const;

    void UpdateTranslation_X(float value);
    void UpdateRotation_X(float value);
    void UpdateScale_X(float value);

    void UpdateTranslation_Y(float value);
    void UpdateRotation_Y(float value);
    void UpdateScale_Y(float value);

    void UpdateTranslation_Z(float value);
    void UpdateRotation_Z(float value);
    void UpdateScale_Z(float value);

    void UpdateTranslation(const glm::vec3& value);
    void UpdateRotation(const glm::vec3& value);
    void UpdateScale(const glm::vec3& value);
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(TransformComponent);
  private:
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

  class ScriptableEntity;
  struct NativeScriptComponent {
    std::unordered_map<std::string, ScriptableEntity*> scrip_name_map;
    ScriptLoaderFn loader_function;
    
    template<typename T, typename... Args>
    void Bind(Args... args) {
      ScriptableEntity* instance = static_cast<ScriptableEntity*>(new T(std::forward<Args>(args)...));
      
      // Store the script name
      int32_t status;
      std::string tname = typeid(T).name();
      char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
      scrip_name_map[(std::string)demangled_name] = instance;

      // Delete the allocated memory
      if(status == 0) {
        tname = demangled_name;
        std::free(demangled_name);
      }
    }
    
    void RenderGui();
    
    NativeScriptComponent(ScriptLoaderFn loader_fun = nullptr);
    ~NativeScriptComponent();
    DEFINE_COPY_MOVE_CONSTRUCTORS(NativeScriptComponent);
  };
  
  struct RigidBodyComponent {
    enum class Type : uint8_t {
      AABB, Sphere, Point
    };
    
    Type type;
    AABB aabb;
    
    void RenderGui();
    RigidBodyComponent(Type type);
    DEFINE_COPY_MOVE_CONSTRUCTORS(RigidBodyComponent);
  };

}