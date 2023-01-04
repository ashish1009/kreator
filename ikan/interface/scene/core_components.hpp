//
//  core_components.hpp
//  ikan
//
//  Created by Ashish . on 04/01/23.
//

#pragma once

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
    
    DEFINE_COPY_MOVE_CONSTRUCTORS(TransformComponent);
  };
  
}
