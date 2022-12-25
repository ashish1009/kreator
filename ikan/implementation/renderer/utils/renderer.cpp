//
//  renderer.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "renderer.hpp"
#include "renderer/utils/renderer_api.hpp"
#include "imgui/imgui_api.hpp"

namespace ikan {
  
  namespace renderer_utils {
    
#ifdef IK_DEBUG_FEATURE
    std::string GetRendererApiName(Renderer::Api api) {
      std::string renderer_api_name = "None";
      switch (api) {
        case Renderer::Api::OpenGl : renderer_api_name = "Open GL"; break;
        case Renderer::Api::None:
        default:
          IK_CORE_ASSERT(false, "Invalid Renderer API");
      }
      return renderer_api_name;
    }
#endif
    
  }

  struct RendererData {
    friend class Renderer;
  public:
    Renderer::Api api = Renderer::Api::None;
    std::unique_ptr<RendererAPI> renderer_api_instance;

    /// This function uodate the renderer API
    /// - Parameter new_api: Rendere APU
    void SetApi(Renderer::Api new_api) {
      IK_CORE_INFO("Rsetting the Renderer API as {0} ...", renderer_utils::GetRendererApiName(api));
      api = new_api;
      renderer_api_instance = RendererAPI::Create();
    }
    
  private:
    RendererData(Renderer::Api new_api)
    : api(new_api) {
      IK_CORE_TRACE("Creating Renderer Data Instance with {0} API ...", renderer_utils::GetRendererApiName(api));
    }
    ~RendererData() {
      IK_CORE_WARN("Destroying Renderer Data Instance with API {0} !!!", renderer_utils::GetRendererApiName(api));
    }
    
    DELETE_COPY_MOVE_CONSTRUCTORS(RendererData);
  };
  
  RendererData* Renderer::renderer_data_ = nullptr;
  
  // -------------------------------------------------------------------------
  // Fundamentals
  // -------------------------------------------------------------------------
  void Renderer::CreateRendererData(Renderer::Api api) {
    renderer_data_ = new RendererData(api);
  }
  
  void Renderer::Initialize() {
    // Create Renderer Data
    renderer_data_->renderer_api_instance = RendererAPI::Create();
  }
  
  void Renderer::Shutdown() {
    delete renderer_data_;
  }
  
  // -------------------------------------------------------------------------
  // Renderer API
  // -------------------------------------------------------------------------
  void Renderer::SetApi(Renderer::Api api) {
    renderer_data_->SetApi(api);
  }
  
  Renderer::Api Renderer::GetApi() {
    return renderer_data_->api;
  }
  
  void Renderer::Framerate() {
    ImguiAPI::Framerate();
  }
  
  // -------------------------------------------------------------------------
  // Renderer Capabilities
  // -------------------------------------------------------------------------
  Renderer::Capabilities& Renderer::Capabilities::Get() {
    static Capabilities capabilities;
    return capabilities;
  }
  
  void Renderer::Capabilities::Log() {
    IK_CORE_INFO("  Renderer Capability | Value ");
    IK_CORE_INFO("  ----------------------------");
    IK_CORE_INFO("  Vendor              | {0} ", vendor);
    IK_CORE_INFO("  Renderer            | {0} ", renderer);
    IK_CORE_INFO("  Version             | {0} ", version);
  }

}
