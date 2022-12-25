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
  }
  
  Application::~Application() {
    IK_CORE_WARN("Destroying Core Application Instance !!!");
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
  : name(other.name), client_asset_path(other.client_asset_path) {
    IK_CORE_TRACE("Copying Application Specification ...");
  }
  
  Application::Specification::Specification(Application::Specification&& other)
  : name(other.name), client_asset_path(other.client_asset_path) {
    IK_CORE_TRACE("Moving Application Specification ...");
  }
  
  Application::Specification&
  Application::Specification::operator=(const Application::Specification& other) {
    IK_CORE_TRACE("Copying Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    return *this;
  }
  
  Application::Specification&
  Application::Specification::operator =(Application::Specification&& other) {
    IK_CORE_TRACE("Moving Application Specification using operator = ...");
    name = other.name;
    client_asset_path = other.client_asset_path;
    return *this;
  }
  
  void Application::Specification::Log() {
    IK_CORE_INFO("  Application Property | Value ");
    IK_CORE_INFO("  -----------------------------");
    IK_CORE_INFO("  Name                 | {0}", name);
    IK_CORE_INFO("  Client Asset Path    | {0}", client_asset_path.c_str());
  }

}
