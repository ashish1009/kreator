//
//  renderer.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "renderer.hpp"
#include "imgui/imgui_api.hpp"
#include "renderer/graphics/shader.hpp"
#include "renderer/graphics/texture.hpp"
#include "renderer/graphics/pipeline.hpp"
#include "renderer/utils/renderer_stats.hpp"
#include "renderer/utils/batch_2d_renderer.hpp"

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
    
    // Initialize Renderers
    BatchRenderer::Init();
  }
  
  void Renderer::Shutdown() {
    ShaderLibrary::ResetShaders();
    BatchRenderer::Shutdown();
    
    delete renderer_data_;
  }
  
  // -------------------------------------------------------------------------
  // Fundamental Renderer API
  // -------------------------------------------------------------------------
  void Renderer::Clear(const glm::vec4& color) {
    renderer_data_->renderer_api_instance->SetClearColor(color);
    renderer_data_->renderer_api_instance->ClearBits();
  }
  
  void Renderer::SetClearColor(const glm::vec4& color) {
    renderer_data_->renderer_api_instance->SetClearColor(color);
  }
  
  void Renderer::ClearBits() {
    renderer_data_->renderer_api_instance->ClearBits();
  }
  void Renderer::ClearDepthBit() {
    renderer_data_->renderer_api_instance->ClearDepthBit();
  }
  void Renderer::ClearColorBit() {
    renderer_data_->renderer_api_instance->ClearColorBit();
  }
  void Renderer::ClearStencilBit() {
    renderer_data_->renderer_api_instance->ClearStencilBit();
  }
  
  void Renderer::Depth(bool state) {
    renderer_data_->renderer_api_instance->Depth(state);
  }
  void Renderer::Blend(bool state) {
    renderer_data_->renderer_api_instance->Blend(state);
  }
  void Renderer::MultiSample(bool state) {
    renderer_data_->renderer_api_instance->MultiSample(state);
  }
  
  void Renderer::DepthFunc(RendererAPI::GlFunc func) {
    renderer_data_->renderer_api_instance->DepthFunc(func);
  }
  
  void Renderer::BeginWireframe() {
    renderer_data_->renderer_api_instance->BeginWireframe();
  }
  void Renderer::EndWireframe() {
    renderer_data_->renderer_api_instance->EndWireframe();
  }
  
  void Renderer::SetViewport(uint32_t width, uint32_t height) {
    renderer_data_->renderer_api_instance->SetViewport(width, height);
  }
  
  void Renderer::GetEntityIdFromPixels(int32_t mx,
                                       int32_t my,
                                       uint32_t pixel_id_index,
                                       int32_t& pixel_data) {
    renderer_data_->renderer_api_instance->GetEntityIdFromPixels(mx,
                                                                 my,
                                                                 pixel_id_index,
                                                                 pixel_data);
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
  
  // -------------------------------------------------------------------------
  // Renderer Stats API
  // -------------------------------------------------------------------------
  void Renderer::ResetStatsEachFrame() {
    RendererStatistics::Get().ResetEachFrame();
  }
  
  void Renderer::ResetStats() {
    RendererStatistics::Get().ResetAll();
  }
  
  void Renderer::RenderStatsGui(bool summary) {
    RendererStatistics::Get().RenderGui(summary);
  }
  
  // -----------------
  // Gui Widgets
  // -----------------
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

  // -------------------------------------------------------------------------
  // Library
  // -------------------------------------------------------------------------
  std::shared_ptr<Shader> Renderer::GetShader(const std::string& path) {
    return ShaderLibrary::GetShader(path);
  }
  std::shared_ptr<Texture> Renderer::GetTexture(const std::string& path,
                                               bool min_linear,
                                               bool mag_linear) {
    return TextureLibrary::GetShader(path, min_linear, mag_linear);
  }

  // -------------------------------------------------------------------------
  // Draw APIs
  // -------------------------------------------------------------------------
  void Renderer::DrawIndexed(const std::shared_ptr<Pipeline>& pipeline, uint32_t count) {
    renderer_data_->renderer_api_instance->DrawIndexed(pipeline, count);
  }

}
