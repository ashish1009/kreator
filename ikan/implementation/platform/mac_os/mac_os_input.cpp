//
//  input.cpp
//  ikan
//
//  Created by Ashish . on 27/12/22.
//

#include "input.hpp"
#include "core/core_application.hpp"

#include <GLFW/glfw3.h>

namespace ikan {
  
  bool Input::IsKeyPressed(KeyCode key) {
    auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    auto state   = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_PRESS or state == GLFW_REPEAT;
  }
  
  bool Input::IsMouseButtonPressed(MouseButton button) {
    auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    auto state   = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
  }
  
  bool Input::IsKeyReleased(KeyCode key) {
    auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    auto state   = glfwGetKey(window, static_cast<int32_t>(key));
    return state == GLFW_RELEASE;
  }
  
  bool Input::IsMouseButtonReleased(MouseButton button) {
    auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    auto state   = glfwGetMouseButton(window, static_cast<int32_t>(button));
    return state == GLFW_RELEASE;
  }
  
  glm::vec2 Input::GetMousePosition() {
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    return { (float)xpos, (float)ypos };
  }
  
  float Input::GetMouseX() {
    return GetMousePosition().x;
  }
  
  float Input::GetMouseY() {
    return GetMousePosition().y;
  }
  
  void Input::SetCursorMode(CursorMode mode) {
    auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindowPtr());
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
  }
  
} 
