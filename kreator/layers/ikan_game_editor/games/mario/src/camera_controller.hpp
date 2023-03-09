//
//  camera_controller.hpp
//  kreator
//
//  Created by Ashish . on 09/03/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CameraController : public ScriptableEntity {
  public:
    void Update(Timestep ts) override;
    void RenderGui() override;
    void Copy(void* script) override {
      if (!script)
        return;
      
      CameraController* camera_script = reinterpret_cast<CameraController*>(script);
      IK_ASSERT(camera_script);
      
      speed_ = camera_script->speed_;
    }
    
  private:
    float speed_ = 10.0f;
  };
  
}
