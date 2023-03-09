//
//  camera_controller.cpp
//  kreator
//
//  Created by Ashish . on 09/03/23.
//

#include "camera_controller.hpp"

namespace mario {
  
  void CameraController::Update(Timestep ts) {
//    auto& tc = GetComponent<TransformComponent>();
//
//    if (Input::IsKeyPressed(KeyCode::Left))
//      tc.UpdateTranslation_X(tc.Translation().x - (speed_ * ts));
//    if (Input::IsKeyPressed(KeyCode::Right))
//      tc.UpdateTranslation_X(tc.Translation().x + (speed_ * ts));
  }
  void CameraController::RenderGui() {
    PropertyGrid::Float1("Speed", speed_);
  }
  
}
