//
//  scriptable_entity.h
//  ikan
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

#include "scene/entity.hpp"
#include "core/math/aabb.hpp"

namespace ikan {
  
  class NativeScriptComponent;
  
  class ScriptableEntity {
  public:
    virtual ~ScriptableEntity() {}

    template<typename T>
    T& GetComponent() {
      return entity_.GetComponent<T>();
    }
    
    virtual void RenderGui() {}

  protected:
    virtual void Create() {}
    virtual void Destroy() {}
    virtual void Update(Timestep ts) {}
    
    /// This function detects the collistion of entity_ with AABB
    /// - Parameter aabb: AABB of bouding box
    bool CollisionDetected(const AABB& aabb);

  protected:
    Entity entity_;
    EnttScene* scene_;
    friend class EnttScene;
  };
  
  class FreeFallController : public ScriptableEntity {
  public:
    void Update(Timestep ts) override;
    
  private:
    float speed_ = 5.0f;
  };
  
}

namespace ikan {
  
  // ---------------------------------------------------------------------------
  // This file holds all the native scripts. For now we are using only C type
  // Scripts so we have to pre define all of them. No Runtime script is supported
  // yet. So store all the script name as their class name in ScriptManager::scripts_
  // ---------------------------------------------------------------------------
  // Using Typedefs
  using ScriptLoaderFn = std::function<bool(NativeScriptComponent* sc, const std::string& script_name)>;  
  class ScriptManager {
  public:
    /// This function update the scrip bind function pointer
    /// - Parameters:
    ///   - sc: Native script component pointer
    ///   - script_name: Script name
    static void UpdateScript(NativeScriptComponent* sc,
                             const std::string& script_name,
                             ScriptLoaderFn loader_function);
    
    MAKE_PURE_STATIC(ScriptManager);
  };
  
}
