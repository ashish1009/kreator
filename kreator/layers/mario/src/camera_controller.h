//
//  camera_controller.h
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

namespace mario {
  
  using namespace ikan;
  
  class CameraController : public ScriptableEntity {
  public:
    CameraController(float speed) : speed_(speed) { }
    
    void Update(Timestep ts) override {
      // Dummy copy of entity y Position
      auto& tc = GetComponent<TransformComponent>();
      if (Input::IsKeyPressed(KeyCode::Left))
        tc.UpdateTranslation_X(tc.Translation().x - (speed_ * ts));
      if (Input::IsKeyPressed(KeyCode::Right))
        tc.UpdateTranslation_X(tc.Translation().x + (speed_ * ts));
    }
    
  private:
    float speed_ = 25.0f;
  };
  
}
