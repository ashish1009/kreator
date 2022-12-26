//
//  core_application.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "core_application.hpp"

namespace ikan {
  
  namespace applicaiton_utils {
    
#ifdef IK_DEBUG_FEATURE
    /// This function returns the Operating System Type as string
    /// - Parameter os: OS Type in iKan Enum
    static std::string GetOsNameAsString(OperatingSystem os) {
      switch (os) {
        case OperatingSystem::Mac : return "MAC OS";
        case OperatingSystem::None :
        default:
          IK_ASSERT(false, "Invalid OS Type");
      }
    }
#endif
    
  } // namespace applicaiton_utils
  
  Application *Application::instance_ = nullptr;

  Application::Application(const Specification& spec)
  : specification_(spec) {
    // If instance already created then abort the application as multiple
    // instacnes should not be therer
    IK_CORE_ASSERT(!instance_, "Application already exists !!!");
    instance_ = this;

    IK_CORE_INFO("Creating Core Application Instance ...");
    specification_.Log();
    
    AssetManager::SetClientAssetPath(specification_.client_asset_path);
    
    // Create Memroy for Renderer Data
    // NOTE: Creating the Renderer Data Memory in very begining as this will
    // setup the Renderer API to be used to create any Renderer Implementation
    Renderer::CreateRendererData(specification_.rendering_api);

    // Create window instance
    window_ = Window::Create(specification_.os, specification_.window_specification);
    
    // Set the application callback to window
    window_->SetEventFunction(IK_BIND_EVENT_FN(Application::EventHandler));
    
    // Decorate Window
    window_->SetResizable(specification_.resizable);
    if (specification_.start_maximized)
      window_->Maximize();
    
    // Initialize the Imgui Layer if GUI is enabled
    if (specification_.enable_gui) {
      imgui_layer_ = ImguiLayer::Create((void*)GetWindow().GetNativeWindow());
      PushLayer(imgui_layer_);
    }

    // Create Memroy for Renderer Data
    Renderer::Initialize();
    
    IK_CORE_INFO(" =============  Core Application Initialized  =============");
  }
  
  Application::~Application() {
    IK_CORE_WARN("Destroying Core Application Instance !!!");
    Renderer::Shutdown();
  }
  
  void Application::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(IK_BIND_EVENT_FN(Application::WindowClose));
    // Event Handler for all layers
    for (auto& layer : layer_stack_)
      layer->EventHandler(event);
  }
  
  bool Application::WindowClose([[maybe_unused]]
                                WindowCloseEvent& window_close_event) {
    Close();
    return false;
  }
    
  void Application::Run() {
    IK_CORE_INFO(" ==================  Starting Game Loop  ==================");
    while(is_running_) {
      // Store the frame time difference
      time_step_ = window_->GetTimestep();
      
      // Window update each frame
      window_->Update();
      
      // Updating all the attached layer
      for (auto& layer : layer_stack_)
        layer->Update(time_step_);
      
      if (specification_.enable_gui)
        RenderGui();
    }

    IK_CORE_INFO(" ==================   Ending Game Loop   ==================");
  }
  
  void Application::RenderGui() {
    imgui_layer_->Begin();

    // Updating all the attached layer
    for (auto& layer : layer_stack_)
      layer->RenderGui();
    
    imgui_layer_->End();
  }
  
  void Application::Close() { is_running_ = false; }
  void Application::PushLayer(const std::shared_ptr<Layer>& layer) { layer_stack_.PushLayer(layer); }
  void Application::PopLayer(const std::shared_ptr<Layer>& layer) { layer_stack_.PopLayer(layer); }
  void* Application::GetWindowPtr() const { return (void*)window_->GetNativeWindow(); }
  const Window& Application::GetWindow() const { return *window_; }
  ImguiLayer& Application::GetImGuiLayer() const { return *imgui_layer_; }
  Application& Application::Get() { return *instance_; }

  // --------------------------------------------------------------------------
  // Application Specification
  // --------------------------------------------------------------------------
  Application::Specification::Specification() {
    IK_CORE_TRACE("Creating Application Specification ... ");
  }

  Application::Specification::~Specification() {
    IK_CORE_WARN("Destroying Application Specification !!!");
  }
  
  Application::Specification::Specification(const Application::Specification& other)
  : name(other.name), client_asset_path(other.client_asset_path),
  rendering_api(other.rendering_api), window_specification(other.window_specification),
  os(other.os), start_maximized(other.start_maximized), resizable(other.resizable),
  enable_gui(other.enable_gui) {
    IK_CORE_TRACE("Copying Application Specification ...");
  }
  
  Application::Specification::Specification(Application::Specification&& other)
  : name(other.name), client_asset_path(other.client_asset_path),
  rendering_api(other.rendering_api), os(other.os), window_specification(other.window_specification),
  start_maximized(other.start_maximized), resizable(other.resizable), enable_gui(other.enable_gui) {
    IK_CORE_TRACE("Moving Application Specification ...");
  }
  
  Application::Specification&
  Application::Specification::operator=(const Application::Specification& other) {
    IK_CORE_TRACE("Copying Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    rendering_api = other.rendering_api;
    os = other.os;
    window_specification = other.window_specification;
    start_maximized = other.start_maximized;
    resizable = other.resizable;
    enable_gui = other.enable_gui;

    return *this;
  }
  
  Application::Specification&
  Application::Specification::operator =(Application::Specification&& other) {
    IK_CORE_TRACE("Moving Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    rendering_api = other.rendering_api;
    os = other.os;
    window_specification = other.window_specification;
    start_maximized = other.start_maximized;
    resizable = other.resizable;
    enable_gui = other.enable_gui;

    return *this;
  }
  
  void Application::Specification::Log() {
    IK_CORE_INFO("  Application Property | Value ");
    IK_CORE_INFO("  -----------------------------");
    IK_CORE_INFO("  Name                 | {0}", name);
    IK_CORE_INFO("  Client Asset Path    | {0}", client_asset_path.c_str());
    IK_CORE_INFO("  Rewndering API       | {0}", renderer_utils::GetRendererApiName(rendering_api));
    IK_CORE_INFO("  Operating System     | {0}", applicaiton_utils::GetOsNameAsString(os));
    IK_CORE_INFO("  Window Maximized     | {0}", start_maximized);
    IK_CORE_INFO("  Window Resizable     | {0}", resizable);
  }

}
