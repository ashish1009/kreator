//
//  scriptable_entity.h
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

#include "ecs/entity.hpp"
#include "core/math/aabb.hpp"
#include "core/math/bounding_circle.hpp"

namespace ecs {
  
  using namespace ikan;
  
  class NativeScriptComponent;

  // Using Typedefs
  using ScriptLoaderFn = std::function<bool(NativeScriptComponent* sc, const std::string& script_name)>;

  // ---------------------------------------------------------------------------
  // This file holds all the native scripts. For now we are using only C type
  // Scripts so we have to pre define all of them. No Runtime script is supported
  // yet. So store all the script name as their class name in ScriptManager::scripts_
  // ---------------------------------------------------------------------------
  class ScriptManager {
  public:
    static std::vector<std::string> scripts_;
    
    /// This function update the scrip bind function pointer
    /// - Parameters:
    ///   - sc: Native script component pointer
    ///   - script_name: Script name
    static void UpdateScript(NativeScriptComponent* sc,
                             const std::string& script_name,
                             ScriptLoaderFn loader_function);
    
    MAKE_PURE_STATIC(ScriptManager);
  };
  
  class ScriptableEntity {
  public:
    template <typename... Args>
    ScriptableEntity(Args... args) {
      
    }
    virtual ~ScriptableEntity() {}

    template<typename T> T& GetComponent() { return entity_.GetComponent<T>(); }
    template<typename T> bool HasComponent() { return entity_.HasComponent<T>(); }

    virtual void RenderGui() {}

  protected:
    virtual void Create() {}
    virtual void Destroy() {}
    virtual void Update(Timestep ts) {}
    
  protected:
    Entity entity_;
    EnttScene* scene_;
    friend class EnttScene;
  };
  
  class NativeMovementController : public ScriptableEntity {
  public:
    void Update(Timestep ts) override;
    void RenderGui() override;
    
    /// This function detects the collistion of entity_ aabb with aabb
    /// - Parameter aabb: AABB of bouding box
    bool CollisionDetected(const AABB& aabb);
    /// This function detects the collistion of entity_ circle with aabb
    /// - Parameter cirlce: circle of bouding box
    bool CollisionDetected(const BoundingCircle& cirlce);

  private:
    float speed_ = 10.0f;
  };

}
