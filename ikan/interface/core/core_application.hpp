//
//  core_application.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

#include "core/events/application_event.hpp"
#include "core/layer_stack.hpp"
#include "core/glfw_window.hpp"
#include "renderer/utils/renderer.hpp"
#include "imgui/imgui_layer.hpp"

namespace ikan {
  
  /// This class is the base class of any application that will be running in the client side. This class
  /// is responsible for
  ///   - Creating Window for the current supported OS
  ///   - Initialize the renderer for the current renderer API
  ///   - Add and store Layer for rendering or any other operations
  ///   - Render the GUI window
  ///   - Handle all the events of the windows Idly events should be handled at Window class only
  ///     but functionlity depends on the application (passed as function pointer)
  /// NOTE: Client application should be derived from this Base appliction if they are including the
  /// ikan Entry point. As ikan Entry point is working on application of type "ikan::Applciation"
  class Application {
  public:
    /// This structure holds the specification of application provided by client
    struct Specification {
      std::string name = "iKan";
      std::string client_asset_path;
      Renderer::Api rendering_api = Renderer::Api::None;
      OperatingSystem os = OperatingSystem::None;
      Window::Specification window_specification;
      bool start_maximized = true;
      bool resizable = true;
      bool enable_gui = true;

      // default constructor and destructors
      Specification();
      ~Specification();
      
      void Log();
      
      // Copy and Move Constructor and operator =
      DEFINE_COPY_MOVE_CONSTRUCTORS(Specification);
    };

    // -----------------------------
    // Constructor and Destructor
    // -----------------------------
    /// This constructs the core application instance
    /// - Parameter spec: application_specification
    Application(const Specification& spec);
    /// This destroys the core application instance
    virtual ~Application();
    
    // ------------------------------------------------------------------------
    // virtual APIs
    // NOTE: Override these virtual Methods in client Application only if you
    // want to create complete fresh application and add some specialisation
    // functionality. If these methods will be overriden in client side then
    // functionality will be completely based on overriden methods
    // ------------------------------------------------------------------------
    
    /// This function is responsible for :
    ///   - Updating the Application.
    ///   - Updating each Layer frame.
    ///   - Updating the window and Swap the buffers
    ///   - Render the GUI by calling 'RenderGui()'
    /// This API performs all the above actions for each API. In Core ikan::entry_point this funciton is
    /// called by default
    virtual void Run();
    /// This Function responsible for closing the current applciation (ikan::Applciation) adn ends the
    /// game loop
    virtual void Close();
    
    // -------------
    // Fundamentals
    // -------------
    /// This function handles all the events of window. Application is dispatching the events and
    /// perform the interupt actions
    /// - Parameter event: event abstract type
    void EventHandler(Event& event);
    /// This function Push the layer of type ikan::Layer in Core Application layer stack. Also attach
    /// the layer (initialise it)
    /// - Parameter layer: Layer Reference pointer to be added
    void PushLayer(const std::shared_ptr<Layer>& layer);
    /// This function Pop the layer of type ikan::Layer from Core Application layer stack. Also detach
    /// the layer (destroy it)
    /// - Parameter layer: Layer Reference pointer to be removed
    void PopLayer(const std::shared_ptr<Layer>& layer);

    // -----------------
    // Getters
    // -----------------
    /// This fuinction returns the native pointer Window native as void
    void* GetWindowPtr() const;
    /// This fuinction returns the iKan Window Instance as reference
    const Window& GetWindow() const;
    /// This fuinction returns the Imgui Layer pointer Reference
    ImguiLayer& GetImGuiLayer() const;

    /// This fuinction returns the reference instance of application
    static Application& Get();

    /// Make Application singleton
    DELETE_COPY_MOVE_CONSTRUCTORS(Application);
    
  private:
    // -----------------
    // Member Functions
    // -----------------
    /// This function dispatched in event dispatcher and trigger when window close event evoked
    /// - Parameter windowEvent: Window close event instacnce
    bool WindowClose([[maybe_unused]] WindowCloseEvent& windowEvent);
    
    /// This function begin the Imgui Renderer and render IMGUI for all the layers and finally ends
    /// the imgui rendering
    void RenderGui();

    // -----------------
    // Member variable
    // -----------------
    Application::Specification specification_;
    LayerStack layer_stack_;
    Timestep time_step_;
    std::unique_ptr<Window> window_;
    std::shared_ptr<ImguiLayer> imgui_layer_;
    bool is_running_ = true;
    
    /// Static instance of singleton Application
    static Application *instance_;
  };
  
  /// Defination API for creating application instance.
  /// NOTE: only defination is defined in core. Declaration should be at client
  /// side based on the application they want to create
  std::unique_ptr<Application> CreateApplication();
  
}
