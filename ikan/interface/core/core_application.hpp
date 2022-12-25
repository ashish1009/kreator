//
//  core_application.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

#include "core/layer_stack.hpp"

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

      // default constructor and destructors
      Specification();
      ~Specification();
      
      /// This function logs the application specification
      void Log();
      
      // Copy and Move Constructor and operator =
      DEFINE_COPY_MOVE_CONSTRUCTORS(Specification);
    };

    // Constructor and Destructor
    /// This constructs the core application instance
    /// - Parameter spec: application_specification
    Application(const Specification& spec);
    /// This destroys the core application instance
    virtual ~Application();
    
    /// This fuinction returns the reference instance of application
    static Application& Get();

    /// Make Application singleton
    DELETE_COPY_MOVE_CONSTRUCTORS(Application);
    
  private:
    Application::Specification specification_;
    LayerStack layer_stack_;
    
    /// Static instance of singleton Application
    static Application *instance_;
  };
  
  /// Defination API for creating application instance.
  /// NOTE: only defination is defined in core. Declaration should be at client
  /// side based on the application they want to create
  std::unique_ptr<Application> CreateApplication();
  
}
