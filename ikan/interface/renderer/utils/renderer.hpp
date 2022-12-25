//
//  renderer.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the wrapper interface of Renderer. That interact with all
// the modules which is responsible for Rendering. Client can use these APIs top
// manage all the renderers

namespace ikan {
  
  /// All renderer ID type
  using RendererID = uint32_t;

  // Forward declarations
  class RendererData;

  /// This class is the wrapper class to interact with all the module that will be needed to render any
  /// object or control any renderer API
  ///   - Create instance of renderer API
  ///   - Initialize any user defined rednerer
  ///   - Contain API to be used by Core or client
  ///   - Shut down or destroy all the renderer data that were created by Renderer
  ///   - Manage the shader and texture library
  class Renderer {
  public:
    /// Supported APIs in ikan engine
    enum class Api : uint8_t {
      None, OpenGl /* Add other supported API with time */
    };

    /// Stores the capability of renderer
    struct Capabilities {
      std::string vendor;
      std::string renderer;
      std::string version;
      
      void Log();
      static Capabilities& Get();
      
    private:
      ~Capabilities() = default;
      Capabilities() = default;
      
      DELETE_COPY_MOVE_CONSTRUCTORS(Capabilities);
    };
    
    // -------------
    // Fundamentals
    // -------------
    /// This function Creates memory for rendere Data. To be called before initializing any open GL
    /// or GLFW instances
    /// - Parameter api: current renderer API
    static void CreateRendererData(Api api);
    /// This function initializes all the renderers including user defined renderer and also setup the
    /// current Renderer API as 'api'. To be called after initializing any open GL or GLFW instances
    static void Initialize();
    /// This function shut down all the renderers including user defined renderer. Also destroy the
    /// instance created for any Graphics API Renderer Should be shutdown after all the layers get
    /// attached as there might be some renderer command still in queue that need to be called in
    /// the end before shutting down the renderer.
    static void Shutdown();

    // --------------------
    // Renderer API
    // --------------------
    /// This function set the current renderer API and initialize the Renderer API instance according
    /// to the supported 'api' NOTE: call this API in the very beginning of the Application as all the
    /// renderer instances (including Windows that use Renderer Context) needs the Supported API.
    /// - Parameter api: New Supported API
    static void SetApi(Api api);
    /// This function returns the current API supported
    static Api GetApi();

  private:
    // -------------------
    // Member veriables
    // -------------------
    static RendererData* renderer_data_;
    MAKE_PURE_STATIC(Renderer);
  };
  
  namespace renderer_utils {
    
#ifdef IK_DEBUG_FEATURE
    /// This function returns the Renderer API name in string
    /// - Parameter api: renderer API in enum format
    std::string GetRendererApiName(Renderer::Api api);
#endif
    
  } // namespace renderer_utils

  
}
