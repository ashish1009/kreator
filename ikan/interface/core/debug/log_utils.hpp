//
//  log_utils.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

namespace ikan {
  
  enum class LogModule {
    None,
    
    // Core
    Assert, Event, AssetManager, Window, LayerStack, Application, Profiler, StringUtils,
    
    // Renderer
    Renderer, Batch2DRenderer, Text,
    FrameBuffer, Pipeline, VertexBuffer, IndexBuffer, Shader, Texture, CharTexture, SubTexture, RendererContext,
    
    // Scene
    Entity, Component, EnttScene, SceneCamera,
    
    // Editor
    EditorCamera, ContentBrowserPanel, ScenePanelManager,
    
    // Ray Tracing
    HitPayload, Ray, Sphere, RayMaterial,
    
    // Imgui
    Imgui
  };
  
  static inline std::string GetModuleName(const std::string_view module_tag) {
    return std::string(module_tag);
  }
  
  static inline std::string GetModuleName(LogModule module_tag) {
    switch (module_tag) {
      case LogModule::None: return "";

      case LogModule::Assert: return "Assert";
      case LogModule::Event: return "Event";
      case LogModule::AssetManager: return "Asset Manager";
      case LogModule::Window: return "Window";
      case LogModule::LayerStack: return "Layer Stack";
      case LogModule::Application: return "Application";
      case LogModule::Profiler: return "Profiler";
      case LogModule::StringUtils: return "StringUtils";

      case LogModule::Renderer: return "Renderer";
      case LogModule::Batch2DRenderer: return "Batch 2D";
      case LogModule::Text: return "Text Renderer";
      case LogModule::FrameBuffer: return "FrameBuffer";
      case LogModule::Pipeline: return "Pipeline";
      case LogModule::VertexBuffer: return "Vertex Buffer";
      case LogModule::IndexBuffer: return "Index Buffer";
      case LogModule::Shader: return "Shader";
      case LogModule::Texture: return "Texture";
      case LogModule::CharTexture: return "Char Texture";
      case LogModule::SubTexture: return "Sub Texture";
      case LogModule::RendererContext: return "Renderer Context";

      case LogModule::Entity: return "Entity";
      case LogModule::Component: return "Components";
      case LogModule::EnttScene: return "EnttScene";
      case LogModule::SceneCamera: return "Scene Camera";
        
      case LogModule::EditorCamera: return "Editor Camera";
      case LogModule::ContentBrowserPanel: return "Content Browser";
      case LogModule::ScenePanelManager: return "Scene Panel";
        
      case LogModule::HitPayload: return "HitPayload";
      case LogModule::Ray: return "Ray";
      case LogModule::Sphere: return "Sphere";
      case LogModule::RayMaterial: return "RayMaterial";
        
      case LogModule::Imgui: return "ImGui";

      default: assert(false);
    }
  }
  
}
