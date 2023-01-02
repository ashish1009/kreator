//
//  imgui_layer.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "imgui_layer.hpp"
#include "core/core_application.hpp"
#include "core/events/event.hpp"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

namespace ikan {
  
  ImguiFont::ImguiFont(const std::string& path, float size) : path(path), size(size) {}
  
  ImguiLayer::ImguiLayer(void* window_pointer) : Layer("Imgui Layer"),
  window_pointer_(window_pointer) {
    IK_CORE_TRACE("Creating Imgui Layer instance ...");
  }
  
  ImguiLayer::~ImguiLayer() noexcept {
    IK_CORE_WARN("Destroying Imgui Layer instance !!!");
  }
  
  void ImguiLayer::Attach() {
    IK_CORE_TRACE("'{0}' is attached to application", GetName());
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform
    // windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    GLFWwindow* window = static_cast<GLFWwindow*>(window_pointer_);
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
  }
  
  void ImguiLayer::Detach() {
    IK_CORE_WARN("'{0}' is detached to application", GetName());
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }
  
  void ImguiLayer::EventHandler(Event& event) {
    if (block_events_) {
      ImGuiIO& io = ImGui::GetIO();
      
      event.handled_ |= event.IsInCategory(Event::kEventCategoryMouse) &
      io.WantCaptureMouse;
      event.handled_ |= event.IsInCategory(Event::kEventCategoryKeyboard) &
      io.WantCaptureKeyboard;
    }
  }
  
  void ImguiLayer::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
    ImGui::SaveIniSettingsToDisk("testing.ini");
    
    ImGui::NewFrame();
  }
  
  void ImguiLayer::End() {
    ImGuiIO& io      = ImGui::GetIO();
    const Application& app = Application::Get();
    io.DisplaySize   = ImVec2((float)app.GetWindow().GetWidth(),
                              (float)app.GetWindow().GetHeight());
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  }
  
  void ImguiLayer::SetFont(const ImguiFont &default_font,
                           const ImguiFont &bold_font) {
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Store the Bold font as well
    io.Fonts->AddFontFromFileTTF(bold_font.path.c_str(), bold_font.size);
    
    // Default font is Regular
    io.FontDefault = io.Fonts->AddFontFromFileTTF(default_font.path.c_str(),
                                                  default_font.size);
    
    IK_CORE_INFO("Imgui Font changed");
    IK_CORE_INFO("  Regular Font Path | {0}", default_font.path.c_str());
    IK_CORE_INFO("  Regular Font Size | {0}", default_font.size);
    IK_CORE_INFO("  Bold Font Path    | {0}", bold_font.path.c_str());
    IK_CORE_INFO("  Bold Font Size    | {0}", bold_font.size);
  }
  
  void ImguiLayer::SetIniFilePath(const std::string& ini_file_path) {
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = ini_file_path.c_str();
  }

  // Unused Functions in Imgui for now
  void ImguiLayer::Update([[maybe_unused]] Timestep ts) { }
  void ImguiLayer::RenderGui() {}
  
  std::shared_ptr<ImguiLayer> ImguiLayer::Create(void* window_pointer) {
    return std::make_shared<ImguiLayer>(window_pointer);
  }
  
}
