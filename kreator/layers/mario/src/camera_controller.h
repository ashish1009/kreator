//
//  camera_controller.h
//  kreator
//
//  Created by Ashish . on 07/01/23.
//

#pragma once

namespace mario {
  
  class CameraController : public ScriptableEntity {
  public:
    CameraController(float speed) : speed_(speed) { }
    
    void Update(Timestep ts) override {
      // Dummy copy of entity y Position
      auto& translation = GetComponent<TransformComponent>().translation;
      if (Input::IsKeyPressed(KeyCode::Left))
        translation.x -= speed_ * ts;
      if (Input::IsKeyPressed(KeyCode::Right))
        translation.x += speed_ * ts;
    }
    
  private:
    float speed_ = 25.0f;
  };
  
}
