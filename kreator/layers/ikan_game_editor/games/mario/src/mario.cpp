//
//  mario.cpp
//  kreator
//
//  Created by Ashish . on 15/02/23.
//

#include "mario.hpp"
#include "camera_controller.hpp"

namespace mario {
  
  static const std::string MarioLogTag = "Mario";

  Mario* Mario::instance_ = nullptr;
  
  Mario::Mario(const Viewport* const viewport) {
    IK_ASSERT(!instance_);
    instance_ = this;

    IK_INFO(MarioLogTag, "Creating Mario Game Data ... ");
  }
  
  Mario::~Mario() {
    IK_WARN(MarioLogTag, "Destroying Mario Game Data ... ");
  }
    
  void Mario::Init(const std::shared_ptr<EnttScene> scene, ScenePanelManager* panel) {
    IK_INFO(MarioLogTag, "Initializing Mario Game Data ... ");

    scene_ = scene;
    panel_ = panel;

    CreateOrSearchCamera();
  }
  
  void Mario::Update(Timestep ts) {
  }
  
  void Mario::RenderGui() {
  }
  
  void Mario::EventHandler(Event& event) {
  }
  
  void Mario::SetViewportSize(uint32_t width, uint32_t height) {
  }

  ImguiFont Mario::RegularFontData() const {
    return { AM::ClientAsset(font_path_), 14.0f };
  }
  
  ImguiFont Mario::BoldFontData() const {
    return { AM::ClientAsset(font_path_), 14.0f };
  }

  std::vector<std::filesystem::path> Mario::FavDirecotries() const {
    return {
      AM::ProjectPath("kreator/layers/ikan_game_editor/games/mario/assets"),
      AM::ClientAsset("scenes"),
    };
  }

  void Mario::CreateOrSearchCamera() {
    bool found_camera = false;
    Entity camera_entity;

    auto camera_view = scene_->GetEntitesWith<CameraComponent>();
    for (auto entity : camera_view) {
      auto& camera_comp = camera_view.get<CameraComponent>(entity);
      if (camera_comp.is_primary == true) {
        found_camera = true;
        camera_entity = Entity(entity, scene_.get());
        break;
      }
    }
    
    if (!found_camera) {
      camera_entity = scene_->CreateEntity("Mario Primary Camera");
      auto& cc = camera_entity.AddComponent<CameraComponent>();
      cc.is_primary = true;
    }

    // Mario Camera controler script loader function
    auto camera_controler_loader_fn = [](NativeScriptComponent* sc, const std::string& script_name) {
      if (script_name == "mario::CameraController") {
        sc->Bind<mario::CameraController>();
        return true;
      }
      return false;
    };

    // Add Native script
    if (camera_entity.HasComponent<NativeScriptComponent>()) {
      auto& nsc = camera_entity.GetComponent<NativeScriptComponent>();
      nsc.loader_function = camera_controler_loader_fn;
      nsc.Bind<CameraController>();
    }
    else {
      auto& nsc = camera_entity.AddComponent<NativeScriptComponent>("mario::CameraController", camera_controler_loader_fn);
    }
  }

}
