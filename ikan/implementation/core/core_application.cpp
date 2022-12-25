//
//  core_application.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "core_application.hpp"

namespace ikan {
  
  Application *Application::instance_ = nullptr;

  Application::Application(const Specification& spec)
  : specification_(std::move(spec)) {
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
    
    // Create Memroy for Renderer Data
    Renderer::Initialize();
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
    IK_CORE_INFO(" ------------------  Starting Game Loop  ------------------");
    IK_CORE_INFO(" ------------------   Ending Game Loop   ------------------");
  }
  
  void Application::RenderGui() {
    // Begin the Imgui Layer
    
    // Updating all the attached layer
    for (auto& layer : layer_stack_)
      layer->RenderGui();
    
    // End the Imgui Layer
  }
  
  void Application::Close() {
    is_running_ = false;
  }

  void Application::PushLayer(const std::shared_ptr<Layer>& layer) {
    layer_stack_.PushLayer(layer);
  }
  
  void Application::PopLayer(const std::shared_ptr<Layer>& layer) {
    layer_stack_.PopLayer(layer);
  }
  
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
  rendering_api(other.rendering_api) {
    IK_CORE_TRACE("Copying Application Specification ...");
  }
  
  Application::Specification::Specification(Application::Specification&& other)
  : name(other.name), client_asset_path(other.client_asset_path),
  rendering_api(other.rendering_api) {
    IK_CORE_TRACE("Moving Application Specification ...");
  }
  
  Application::Specification&
  Application::Specification::operator=(const Application::Specification& other) {
    IK_CORE_TRACE("Copying Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    rendering_api = other.rendering_api;
    return *this;
  }
  
  Application::Specification&
  Application::Specification::operator =(Application::Specification&& other) {
    IK_CORE_TRACE("Moving Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    rendering_api = other.rendering_api;
    return *this;
  }
  
  void Application::Specification::Log() {
    IK_CORE_INFO("  Application Property | Value ");
    IK_CORE_INFO("  -----------------------------");
    IK_CORE_INFO("  Name                 | {0}", name);
    IK_CORE_INFO("  Client Asset Path    | {0}", client_asset_path.c_str());
    IK_CORE_INFO("  Rewndering API       | {0}", renderer_utils::GetRendererApiName(rendering_api));
  }

}
