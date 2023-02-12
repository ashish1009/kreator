//
//  core_components.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

#include "renderer/graphics/texture.hpp"
#include "ecs/scriptable_entity.hpp"
#include "core/math/aabb.hpp"
#include "camera/scene_camera.hpp"

#include "box2d/b2_body.h"

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
    TransformComponent(const glm::vec3& translation = { 0.0f, 0.0f, 0.0f });
    void RenderGui();
    
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
    glm::mat4 transform;
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
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
    ScriptableEntity* script;
    ScriptLoaderFn loader_function;
    std::string script_name;
    
    template<typename T, typename... Args>
    void Bind(Args... args) {
      script = static_cast<ScriptableEntity*>(new T(std::forward<Args>(args)...));
      
      // Store the script name
      int32_t status;
      std::string tname = typeid(T).name();
      char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
      script_name = (std::string)demangled_name;

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
    b2BodyType type = b2BodyType::b2_staticBody;
    bool fixed_rotation = false;
    
    glm::vec2 velocity{0, 0};
    float angular_damping = 0.8f;
    float linear_damping = 0.9f;
    float mass = 0;
    float friction = 0.0f;
    float angular_velocity = 0.0f;
    float gravity_scale = 1.0f;
    bool is_sensor = false;
    bool continuous_collision = true;

    // Storage
    b2Body* runtime_body = nullptr;
    
    void AddVelocity();
    void AddImpulse();
    void SetVelocity(const glm::vec2& vel);
    void SetAngularVelocity(float ang_vel);
    void SetGravityScale(float scale);
    void SetIsSensor(bool flag);
    void SetNotSensor(bool flag);
    
    void RenderGui();
    RigidBodyComponent() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(RigidBodyComponent);
  };
  
  struct BoxColloiderComponent {
    glm::vec2 offset = { 0.0f, 0.0f };
    glm::vec2 size = { 0.5f, 0.5f };
    
    // TODO: Move to physics material
    float density  = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float restitution_threshold = 0.5f;

    // Storage for runtime
    void* runtime_fixture = nullptr;

    void RenderGui();
    BoxColloiderComponent() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(BoxColloiderComponent);
  };
  
  struct CircleColloiderComponent {
    glm::vec2 offset = { 0.0f, 0.0f };
    float radius = 0.5f;
    
    // TODO: Move to physics material
    float density  = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float restitution_threshold = 0.5f;

    // Storage for runtime
    void* runtime_fixture = nullptr;

    void RenderGui();
    CircleColloiderComponent() = default;
    DEFINE_COPY_MOVE_CONSTRUCTORS(CircleColloiderComponent);
  };
  
  struct PillBoxCollider {
    float width = 0.5f;
    float height = 0.5f;
    glm::vec2 offset;
    
    BoxColloiderComponent bcc;
    CircleColloiderComponent top_ccc;
    CircleColloiderComponent bottom_ccc;

    void RecalculateColliders();
    
    void RenderGui();
    PillBoxCollider();
    DEFINE_COPY_MOVE_CONSTRUCTORS(PillBoxCollider);
  };
  
  struct AnimationComponent {
    bool animation = true;
    bool is_sprite = false;
    
    // Sprite Data
    int32_t speed = 15;
    int32_t anim_idx = 0; // No need to copy or save in scene. always starts from 0
    std::vector<std::shared_ptr<SubTexture>> sprites;
    std::shared_ptr<Texture> sprite_image; // only for sprite animation
    
    // TODO: Add Data for non_sprites....

    void RenderGui();
    AnimationComponent(std::shared_ptr<Texture> sprite_image);
    DEFINE_COPY_MOVE_CONSTRUCTORS(AnimationComponent);
  };
  
  template<typename... Component>
  struct ComponentGroup {
  };
  
  using AllComponents =
  ComponentGroup<TransformComponent, QuadComponent, CircleComponent, CameraComponent, NativeScriptComponent,
  RigidBodyComponent, BoxColloiderComponent, CircleColloiderComponent, PillBoxCollider, AnimationComponent>;

}
