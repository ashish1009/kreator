//
//  property_grid.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

// This file includes the wrapeer class to suppoer propert grid api

#include <imgui_internal.h>
#include "renderer/graphics/texture.hpp"

namespace ikan {
  
  /// This class stores the Wrapper API for showing Data in Imgui window
  class PropertyGrid {
  public:
    /// This function render a hint message when hovered last imgui item
    /// - Parameters:
    ///   - description: String to be used as hint
    ///   - error: flag to render text in red
    static void HoveredMsg(const char* description,
                           bool error = false);
    /// This function render a hint icon (?) and on hovered A hint string will be shown in subwindow
    /// on hovering the mouse
    /// - Parameters:
    ///   - description: String to be used as hint
    ///   - icon_char: Icon to be printed as Help Marker (By default (?) will be rendered)
    ///   - same_line: check for same line icon or in new line
    static void HelpMarker(const char* description,
                           const char* icon_char = "?",
                           bool same_line = true);
    /// This function renderes a search box with search button
    /// - Parameters:
    ///   - value: value written in the box
    ///   - hint: hint to be shown
    static bool Search(char* value,
                       const char* hint,
                       const glm::vec2& size = { 15.0f, 15.0f });
    /// This function renders Imgae in current Imgui window
    /// - Parameters:
    ///   - textureID; Texture id of Image to be rendered
    ///   - size; size of image need to be rendered inside Imgui window (this size will be visible as texture)
    ///   - uv0; Left
    ///   - uv1; Right
    static void Image(void* textureID,
                      const glm::vec2& size,
                      const glm::vec2& uv0,
                      const glm::vec2& uv1);
    /// This funcrtion renders a Image Button with lable ID
    /// - Parameters:
    ///   - lableId: lable ID
    ///   - texId: Texture ID
    ///   - size: size of textire
    static bool ImageButton(const int32_t lable_id,
                            uint32_t texId,
                            const glm::vec2& size);
    
    /// This function renderes a image button
    /// - Parameters:
    ///   - lableId: image lable string
    ///   - texId: texture id
    ///   - size: size of image
    ///   - bgColor: background color
    ///   - padding: padding
    static bool ImageButton(const std::string& lable_id,
                            uint32_t texId,
                            const glm::vec2& size,
                            const glm::vec4& bgColor = glm::vec4(0.0f),
                            int32_t padding = 0);
    
    /// This function renders slider for float
    /// - Parameters:
    ///   - label: Lable for Variable
    ///   - value: Value reference to be changed
    ///   - checkbox_flag: flag to render either checkbox or lable
    ///   - delta: step of Drag in float
    ///   - INT_MIN: min value
    ///   - INT_MAX: max value
    ///   - reset_value: value of reset button pressed
    ///   - column_width: width of column of lable
    static bool Float1(const char* label,
                       float& value,
                       bool* checkbox_flag = nullptr,
                       float delta = 0.1f,
                       int32_t min_value = INT_MIN,
                       int32_t max_value = INT_MAX,
                       float reset_value = 0.0f,
                       float column_width = 100.0f);

    /// This function renders slider for float 3
    /// - Parameters:
    ///   - label: Lable for Variable
    ///   - value: Value reference to be changed
    ///   - checkbox_flag: flag to render either checkbox or lable
    ///   - delta: step of Drag in float
    ///   - INT_MIN: min value
    ///   - INT_MAX: max value
    ///   - reset_value: value of reset button pressed
    ///   - column_width: width of column of lable
    static bool Float2(const char* label,
                       glm::vec2& value,
                       bool* checkbox_flag = nullptr,
                       float delta = 0.1f,
                       int32_t min_value = INT_MIN,
                       int32_t max_value = INT_MAX,
                       float reset_value = 0.0f,
                       float column_width = 100.0f );
    
    /// This function renders slider for float 3
    /// - Parameters:
    ///   - label: Lable for Variable
    ///   - value: Value reference to be changed
    ///   - checkbox_flag: flag to render either checkbox or lable
    ///   - delta: step of Drag in float
    ///   - INT_MIN: min value
    ///   - INT_MAX: max value
    ///   - reset_value: value of reset button pressed
    ///   - column_width: width of column of lable
    static bool Float3(const char* label,
                       glm::vec3& value,
                       bool* checkbox_flag = nullptr,
                       float delta = 0.1f,
                       int32_t min_value = INT_MIN,
                       int32_t max_value = INT_MAX,
                       float reset_value = 0.0f,
                       float column_width = 100.0f );

    /// This function renders the text box
    /// - Parameters:
    ///   - label: Lable for Variable
    ///   - value: Value need to be edit or entered in box
    ///   - column_width1: width of column of lable
    ///   - column_width2: width of column of Value
    ///   - hint: Hint to be shown in the text box. Null of no hind is given
    ///   - modifiable: flag to check is string is constant or modifiable
    ///   - multiple: flag to check if there are multiple line in text box
    ///   - num_lines: if multipleLine is true then check number of line are there in text box
    ///   - error: flag to check if text is error (if true text printed as red)
    static bool TextBox(std::string& value,
                        const char* label = nullptr,
                        float column_width_1 = 100.0f,
                        float column_width_2 = 300.0f,
                        const char* hint = nullptr,
                        bool modifiable = true,
                        bool multiple = false,
                        int32_t num_lines = 1,
                        bool error = false);
    
    template<typename UIFunction>
    /// This function renders the texture components
    /// - Parameters:
    ///   - texture_comp: texture component
    ///   - color: color of the texture
    ///   - ui_function: function to render below texture Use
    static void RenderTextureComponent(TextureComponent& texture_comp,
                                       glm::vec4& color,
                                       UIFunction ui_function) {
      ImGui::PushID("##PropertyGrid::TextureComponent");
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, 100);
      
      static std::shared_ptr<Texture> no_texture = Renderer::GetTexture(AM::CoreAsset("textures/default/no_texture.png"));
      size_t tex_id = ((texture_comp.component) ? texture_comp.component->GetRendererID() : no_texture->GetRendererID());
      
      // Show the image of texture
      ImGui::Image((void*)tex_id,
                   ImVec2(40.0f, 40.0f),
                   ImVec2(0.0f, 1.0f),
                   ImVec2(1.0f, 0.0f),
                   ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                   ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
      
      // Drop the texture here and load new texture
      PropertyGrid::DropConent([&texture_comp](const std::string& path)
                               {
        texture_comp.component.reset();
        texture_comp.component = Renderer::GetTexture(path);
      });
      PropertyGrid::HoveredMsg("Drop the Texture file in the Image Button to "
                               "upload the texture");
      ImGui::NextColumn();
      
      // Check box to togle use of texture
      ImGui::Checkbox("Use ", &texture_comp.use);
      
      ui_function();
      //    ImGui::ColorEdit4("Color ", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
      
      if (texture_comp.use) {
        ImGui::SameLine();
        ImGui::DragFloat("", &texture_comp.tiling_factor, 1.0f, 1.0f, 1000.0f);
        PropertyGrid::HoveredMsg("Tiling Factor");
      }
      
      ImGui::Columns(1);
      ImGui::Separator();
      ImGui::PopID();
    }
    
    /// This function catch the dragged content from content prowser pannel and call the function passed as ui_function
    /// - Parameter uiFunction: Function
    template<typename UIFunction> static void DropConent(UIFunction ui_function) {
      if (ImGui::BeginDragDropTarget() and
          !ImGui::IsMouseDragging(0) and
          ImGui::IsMouseReleased(0)) {
        if (const ImGuiPayload* data = ImGui::AcceptDragDropPayload("SelectedFile",
                                                                    ImGuiDragDropFlags_AcceptBeforeDelivery)) {
          char* file_path = new char[uint32_t(data->DataSize + 1)];
          memcpy(file_path, (char*)data->Data, (size_t)data->DataSize);
          file_path[data->DataSize] = '\0';
          
          ui_function(file_path);
          delete[] file_path;
        }
        ImGui::EndDragDropTarget();
      }
    }
    
  private:
    static bool FloatImpl(const std::vector<std::string>& buttons,
                          const char* label,
                          const std::vector<float*>& values,
                          bool* checkbox_flag,
                          float delta,
                          int32_t min_value,
                          int32_t max_value,
                          float reset_value,
                          float column_width);

    MAKE_PURE_STATIC(PropertyGrid);
  };
  
}
