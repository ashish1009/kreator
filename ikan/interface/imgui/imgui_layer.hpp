//
//  imgui_layer.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the imgui layer that isused to render the GUI for the engine

#include "core/layer.hpp"

namespace ikan {
  
  /// This structor holds the font information of imgui
  struct ImguiFont {
    std::string path;
    float size;
    
    ImguiFont(const std::string& path, float size);
  };
  
  /// This class creates and manages the Imgui Layer in application NOTE: If Not using iKan Renderer
  /// then create instance of Imgui Layer in Client Application and manage API Accordingly, by default
  /// Application is taking care of all
  class ImguiLayer : public Layer {
  public:
    // --------------------------------------
    // Constructors and Destrcutor
    // --------------------------------------
    /// This constructor creates the Imgui layer instance.
    ImguiLayer(void* window_pointer);
    /// This destrictor destroy the Imgui layer instance
    ~ImguiLayer() noexcept;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(ImguiLayer);
    
    // ------------------
    // Overrides
    // ------------------
    /// This funciton initialises the imgui layer. Creates the imgui layer using imgui file library
    void Attach() override;
    /// This funciton destroy the imgui layer delete all the imgui created impl
    void Detach() override;
    /// This function do nothing for now
    void RenderGui() override;
    /// This function do nothing for now
    void Update([[maybe_unused]] Timestep ts) override;
    /// This function handles all the events triggered for the imgui layer. To be called whenever an
    /// event is triggered in the window NOTE Core ikan::Application is taking care to call the
    /// EventHandler(event) API for all Layers
    /// - Parameter event: Event (Base class) intance. Dispatch event from Event Dispatcher
    void EventHandler(Event& event) override;
    
    // ------------------
    // Fundamentals
    // ------------------
    /// This function begins the imgui rendering for each frame NOTE: Call once per frame
    void Begin();
    /// This function ends the rendering of imgui each frame NOTE: Call once per frame
    void End();
    /// This function blocks the Imgui Events if flag is true
    void BlockEvents(bool);
    /// This function overrides font of imgui with new font path
    /// - Parameters:
    ///   - default_font: Default font informatiom
    ///   - bold_font: bold Font informatiom
    void SetFont(const ImguiFont &default_font, const ImguiFont &bold_font);
    /// This function the save location of ini file path
    /// - Parameter ini_file_path: file path relative to executable
    /// NOTE: Directory must exist.
    void SetIniFilePath(const std::string& ini_file_path);
    
    // -----------------
    // Static funciton
    // -----------------
    /// This static function creates the Ref type instance of Imgui Layer. As all layer to be pushed in
    /// Application stack as Ref.
    static std::shared_ptr<ImguiLayer> Create(void* window_pointer);
    
  private:
    bool block_events_ = true;
    void* window_pointer_ = nullptr;
  };
  
}
