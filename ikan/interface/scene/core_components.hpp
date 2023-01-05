//
//  core_components.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

#include "renderer/graphics/texture.hpp"

namespace ikan {
 
  struct IDComponent {
    UUID id = 0;
    IDComponent(const UUID& id);
    DEFINE_COPY_MOVE_CONSTRUCTORS(IDComponent);
  };

  struct TagComponent {
    std::string tag = "Default Entity";
    TagComponent(const std::string& tag);
    DEFINE_COPY_MOVE_CONSTRUCTORS(TagComponent);
  };
  
  struct TransformComponent {
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    
    TransformComponent(const glm::vec3& translation = { 0.0f, 0.0f, 0.0f });
    glm::mat4 GetTransform() const;
    void RenderGui();

    DEFINE_COPY_MOVE_CONSTRUCTORS(TransformComponent);
  };
  
  struct QuadComponent {
    std::shared_ptr<Texture> texture;
    glm::vec4 color = {0.123f, 0.123f, 0.123f, 1.0f};

    void RenderGui();

    QuadComponent();
    DEFINE_COPY_MOVE_CONSTRUCTORS(QuadComponent);
  };

  struct CircleComponent {
    std::shared_ptr<Texture> texture;
    glm::vec4 color = {0.123f, 0.123f, 0.123f, 1.0f};
    
    float thickness = 1.0f;
    float fade = 0.005f;
    
    void RenderGui();

    CircleComponent();
    DEFINE_COPY_MOVE_CONSTRUCTORS(CircleComponent);
  };

}
