//
//  imgui_api.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "imgui_api.hpp"
#include "editor/property_grid.hpp"
#include "core/core_application.hpp"

namespace ikan {
  
  void ImguiAPI::StartDcocking() {
    // --------------------------------------------------------------------
    // Note: Switch this to true to enable dockspace
    // --------------------------------------------------------------------
    static bool dockspace_open = false;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    
    // --------------------------------------------------------------------
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    // --------------------------------------------------------------------
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    
    if (opt_fullscreen) {
      ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;
    }
    
    // --------------------------------------------------------------------
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin()
    // to not render a background.
    // --------------------------------------------------------------------
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &dockspace_open, window_flags);
    ImGui::PopStyleVar();
    
    if (opt_fullscreen)
      ImGui::PopStyleVar(2);
    
    // ------------
    // DockSpace
    // ------------
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
  }
  
  void ImguiAPI::EndDcocking() {
    ImGui::End();
  }
  
  void ImguiAPI::Framerate(bool *is_open) {
    if (is_open and *is_open == false) return;

    ImGui::Begin("Frame Rate", is_open);
    ImGui::PushID("Frame Rate");
    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    PropertyGrid::HoveredMsg(std::to_string((float)1000.0f/(float)ImGui::GetIO().Framerate).c_str());
    ImGui::PopID();
    ImGui::End();
  }

  void ImguiAPI::ChangeFont(const ImguiFont& default_font_path, const ImguiFont& bold_font_path) {
    auto& imgui_layer = Application::Get().GetImGuiLayer();
    imgui_layer.Detach();
    
    imgui_layer.Attach();
    imgui_layer.SetIniFilePath(Application::Get().GetSpecification().save_ini_file_path);

    imgui_layer.SetFont(default_font_path, bold_font_path);
  }

  void ImguiAPI::SetGreyThemeColors() {
    IK_CORE_TRACE(LogModule::Imgui, "Imgui theme is set to Grey");
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    // Text
    colors[ImGuiCol_Text]                   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    
    // Background
    colors[ImGuiCol_WindowBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    
    // Border
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Frame
    colors[ImGuiCol_FrameBg]                = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
    
    // Tabs
    colors[ImGuiCol_Tab]                    = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered]             = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive]              = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused]           = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    
    // Title
    colors[ImGuiCol_TitleBg]                = ImVec4(0.34f, 0.34f, 0.34f, 1.0f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
    
    // Menu
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    
    // Scroll
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Check
    colors[ImGuiCol_CheckMark]              = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    
    // SLider
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
    
    // Button
    colors[ImGuiCol_Button]                 = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
    
    // Headers
    colors[ImGuiCol_Header]                 = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
    
    // Separator
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Resize
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    
    // Plot
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
    
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
    
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
    
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
  }
  
  void ImguiAPI::SetLightGreyThemeColors() {
    IK_CORE_TRACE(LogModule::Imgui, "Imgui theme is set to Light Grey");
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    // Text
    colors[ImGuiCol_Text]                   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    
    // Background
    colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    
    // Border
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Frame
    colors[ImGuiCol_FrameBg]                = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
    
    // Tabs
    colors[ImGuiCol_Tab]                    = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered]             = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive]              = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused]           = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    
    // Title
    colors[ImGuiCol_TitleBg]                = ImVec4(0.34f, 0.34f, 0.34f, 1.0f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
    
    // Menu
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    
    // Scroll
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.18f, 0.18f, 0.18f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Check
    colors[ImGuiCol_CheckMark]              = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    
    // SLider
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
    
    // Button
    colors[ImGuiCol_Button]                 = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
    
    // Headers
    colors[ImGuiCol_Header]                 = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
    
    // Separator
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Resize
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    
    // Plot
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
    
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
    
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
    
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
  }
  
  void ImguiAPI::SetLightThemeColors() {
    IK_CORE_TRACE(LogModule::Imgui, "Imgui theme is set to Light");
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    // Text
    colors[ImGuiCol_Text]                   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.13f, 0.13f, 0.13f, 0.65f);
    
    // Background
    colors[ImGuiCol_WindowBg]               = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    
    // Border
    colors[ImGuiCol_Border]                 = ImVec4(0.57f, 0.57f, 0.50f, 0.5f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Frame
    colors[ImGuiCol_FrameBg]                = ImVec4(0.7f, 0.7f, 0.7f, 0.5f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.6f, 0.6f, 0.6f, 0.6f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.6f, 0.6f, 0.6f, 0.6f);
    
    // Tabs
    colors[ImGuiCol_Tab]                    = ImVec4{ 0.85f, 0.8495f, 0.849f, 1.0f };
    colors[ImGuiCol_TabHovered]             = ImVec4{ 0.62f, 0.6102f, 0.619f, 1.0f };
    colors[ImGuiCol_TabActive]              = ImVec4{ 0.72f, 0.7105f, 0.719f, 1.0f };
    colors[ImGuiCol_TabUnfocused]           = ImVec4{ 0.85f, 0.8405f, 0.849f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4{ 0.8f, 0.795f, 0.79f, 1.0f };
    
    // Title
    colors[ImGuiCol_TitleBg]                = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.71f, 0.71f, 0.71f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.0f, 1.0f, 1.0f, 0.51f);
    
    // Menu
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
    
    // Scroll
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.59f, 0.59f, 0.59f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.0f);
    
    // Check
    colors[ImGuiCol_CheckMark]              = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    
    // SLider
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.49f, 0.49f, 0.49f, 0.7f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.34f, 0.34f, 0.34f, 1.0f);
    
    // Button
    colors[ImGuiCol_Button]                 = ImVec4(0.56f, 0.56f, 0.56f, 0.4f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.54f, 0.53f, 0.52f, 1.0f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.58f, 0.42f, 0.42f, 1.0f);
    
    // Headers
    colors[ImGuiCol_Header]                 = ImVec4(0.3f, 0.3f, 0.3f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.3f, 0.3f, 0.3f, 0.8f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.52f, 0.5f, 0.48f, 1.0f);
    
    // Separator
    colors[ImGuiCol_Separator]              = ImVec4(0.57f, 0.57f, 0.5f, 0.5f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.28f, 0.28f, 0.28f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.49f, 0.49f, 0.49f, 1.0f);
    
    // Resize
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.09f, 0.09f, 0.09f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.19f, 0.19f, 0.19f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.54f, 0.54f, 0.54f, 0.95f);
    
    // Plot
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.0f, 0.57f, 0.65f, 1.0f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.27f, 0.4f, 0.85f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.0f, 0.4f, 1.0f, 1.0f);
    
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);
    
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.0f, 0.0f, 1.0f, 0.9f);
    
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.40f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
  }
  
  void ImguiAPI::SetDarkThemeColors() {
    IK_CORE_TRACE(LogModule::Imgui, "Imgui theme is set to Dark");
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    // Text
    colors[ImGuiCol_Text]                   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    
    // Background
    colors[ImGuiCol_WindowBg]               = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
    
    // Border
    colors[ImGuiCol_Border]                 = ImVec4(0.33f, 0.33f, 0.40f, 0.5f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Frame
    colors[ImGuiCol_FrameBg]                = ImVec4(0.2f, 0.2f, 0.2f, 0.5f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.3f, 0.3f, 0.3f, 0.4f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.3f, 0.3f, 0.3f, 0.6f);
    
    // Tabs
    colors[ImGuiCol_Tab]                    = ImVec4{ 0.05f, 0.0505f, 0.051f, 1.0f };
    colors[ImGuiCol_TabHovered]             = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabActive]              = ImVec4{ 0.18f, 0.1805f, 0.181f, 1.0f };
    colors[ImGuiCol_TabUnfocused]           = ImVec4{ 0.05f, 0.0505f, 0.051f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
    
    // Title
    colors[ImGuiCol_TitleBg]                = ImVec4(0.24f, 0.24f, 0.24f, 1.0f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.19f, 0.19f, 0.19f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
    
    // Menu
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
    
    // Scroll
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Check
    colors[ImGuiCol_CheckMark]              = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    
    // SLider
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
    
    // Button
    colors[ImGuiCol_Button]                 = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
    
    // Headers
    colors[ImGuiCol_Header]                 = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
    
    // Separator
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    
    // Resize
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    
    // Plot
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
    
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
    
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
    
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
  }
  
}
