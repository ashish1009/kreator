# kreator
A Complete package for rendering. An Engine(ikan) to support render API. Editor to use create application using the 'ikan' Engine

![](/resources/branding/i_kan.png)

## Getting Started
XCode 13 is recommended

You can clone the repository to a local destination using git:

`git clone --recursive https://github.com/ashish1009/kreator`

Make sure that you do a `--recursive` clone to fetch all of the submodules!

## How to Use the library

### Library
`{...} directory where ikan Engine is cloned`
Link the libikan.a in your project... 

### Frameworks
Link following frameworks to your project...

### Header Search Paths
Add the following header search paths 
`{...} directory where ikan Engine is cloned`
${PROJECT_DIR}/../ikan/interface
${PROJECT_DIR}/../ikan/vendor/spd_log/include
${PROJECT_DIR}/../ikan/vendor/glm
${PROJECT_DIR}/../ikan/vendor/imgui

### Library Search Paths
Add the following library search paths 
`{...} directory where ikan Engine is cloned`

### Preprocesor
- Add `IK_DEBUG_FEATURE` to enable Kreator Debug features
- Add `IK_ENABLE_LOG` to enable Logging

## Basic APIs

## TODOs

## Features
- ikan Engine
----------------
  - Fundamentals
    - Logger Framework using SPD log module. API created for both core and client.
    - Assert API created for Trigger Exception in both core and client.
    - Basic Application Class created
    - Layer Base class (Client layer should be derived fom this base class)
    - Layer stack manager to store all the layers in stack
    - Scope/Function profiler Feature addded
  
  - Window
    - Window create feature using GLFW library
  
  - Event Handler
    - Event Abstracetion class
    - Input Handler

  - Fundamental Renderer
    - Interface and Implementation for Renderer Context for attching Renderer to Window
    - Interface and Implementation for Shader Compiler.
    - Interface and Implementation for Shader Uniforms
    - Interface and Implementation for storing Renderer Buffers and Arrays.

  - Editor
    - Added Editor 3D camera

## Road Map  
- ikan Engine
----------------
  - Fundamental Renderer
    - Interface and Implementation for Frame buffer
    - Interface and Implementation for Loading Texture file (using stb image files)
  
  - Materials
    - Added Material class to handle Shader uniforms as collective buffers
    
  - Text Renderer
    - API created to render the Text either via Entity or without entity/Camera 

  - Renderer APIs
    - Batch Renderer API for rendering Quad and Circles (2D Batch)

- ECS Project
----------------
  - Editor
    - Scene Hierarchy pannel : To Grow With Time
      - Property Grid API For IM GUI
      - Content Browser pannel : To Optimize With Time
      - ImGuimzo
        - Mouse selectable entity

  - Math
    - AABB
    - UUID

  - Scene
    - Viewport to load and set the viewport
    - Entity Component System to make User friendly UI
    - Serialise and Deserialize the scene
    - Entity class to store the component of entity
    - Components
    - Scene class to manage all entities

  - 3D Mesh
    - Class to load and Render Model
    - Skeleton
    - Save All Animations
    
  - Environment
    - Light
    - PBR
    - IBL
    - Shadow
    - Gamma
    - Bloom
    - Manager to manage all Environment
    - Normal
    - Parallex
    - Daffered 
    - SSAO

  - Advance Open GL
    - Depth
    - Stencil (outline selected entity)
    - Face culling
    - Advance data
    - Advance GLSL 
    - Instansing (dust and partical)
    - Geomatry shader
    - Anti Aliasing

  - Collision
    - AABB Visual
    - AABB Collision : basic

  - Renderer APIs
    - Scene Renderer
    - API for Environment
    - Render Mesh

- Ray Tracing
----------------
  - Ray Tracing Scene and Renderer
  
- 2D Games
----------------
  - Mario
  - Chess
