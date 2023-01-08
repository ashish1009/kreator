//
//  property_grid.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "property_grid.hpp"

namespace ikan {
  
  void PropertyGrid::HoveredMsg(const char* description, bool error) {
    if (ImGui::IsItemHovered()) {
      if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(description);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
      
      if (error)
        ImGui::PopStyleColor();
    }
  }
  
  void PropertyGrid::HelpMarker(const char* description,
                                const char* icon_char,
                                bool same_line) {
    if (same_line)
      ImGui::SameLine();
    
    ImGui::TextDisabled(icon_char);
    HoveredMsg(description);
  }

  bool PropertyGrid::Search(char* value,
                            const char* hint,
                            const glm::vec2& size) {
    bool modified = false;
    ImGui::PushID("Search");
    ImGui::PushItemWidth(-1);
    
    static std::shared_ptr<Texture> search_exture = Renderer::GetTexture(AM::CoreAsset("textures/icons/search.png"));
    PropertyGrid::Image(INT2VOIDP(search_exture->GetRendererID()),
                        size,
                        glm::vec2( 0, 1 ),
                        glm::vec2( 1, 0 ));
    
    ImGui::SameLine();
    
    // Copy the Name of entity to buffer that will be dumy text in property pannel
    char buffer[256];
    strcpy(buffer, value);
    
    if (ImGui::InputTextWithHint("", hint, buffer, IM_ARRAYSIZE(buffer))) {
      strcpy(value, buffer);
      modified = true;
    }
    
    ImGui::PopItemWidth();
    ImGui::PopID();
    
    return modified;
  }
  
  void PropertyGrid::Image(void* textureID,
                           const glm::vec2& size,
                           const glm::vec2& uv0,
                           const glm::vec2& uv1) {
    auto& colors = ImGui::GetStyle().Colors;
    const auto& button_hovered = colors[ImGuiCol_ButtonHovered];
    
    // ------------------------------------------
    // No Background color
    // ------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(button_hovered.x,
                                 button_hovered.y,
                                 button_hovered.z,
                                 0.5f));
    
    ImGui::Image((void*)textureID,
                 ImVec2{ size.x, size.y },
                 ImVec2{ uv0.x, uv0.y },
                 ImVec2{ uv1.x, uv1.y });
    
    ImGui::PopStyleColor(1);
  }
  
  bool PropertyGrid::ImageButton(int32_t lable_id,
                                 uint32_t texId,
                                 const glm::vec2& size) {
    bool result = false;
    ImTextureID my_texture_id = (ImTextureID)((size_t)texId);
    ImGui::PushID(lable_id);
    result = ImGui::ImageButton(my_texture_id,
                                { size.x, size.y },
                                ImVec2(0.0f, 1.0f),
                                ImVec2(1.0f, 0.0f));
    ImGui::PopID();
    
    return result;
  }
  
  bool PropertyGrid::ImageButton(const std::string& lable_id,
                                 uint32_t texId,
                                 const glm::vec2& size,
                                 const glm::vec4& bgColor,
                                 int32_t padding) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    
    auto& colors = ImGui::GetStyle().Colors;
    const auto& button_hovered = colors[ImGuiCol_ButtonHovered];
    
    // ------------------------------------------
    // No Background color
    // ------------------------------------------
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(button_hovered.x, button_hovered.y, button_hovered.z, 0.5f));
    const auto& button_cctive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(button_cctive.x, button_cctive.y, button_cctive.z, 0.5f));
    
    bool result = false;
    ImTextureID my_texture_id = (ImTextureID)((size_t)texId);
    ImGui::PushID(lable_id.c_str());
    result = ImGui::ImageButton(my_texture_id,
                                { size.x, size.y },
                                ImVec2(0.0f, 1.0f),
                                ImVec2(1.0f, 0.0f),
                                padding,
                                { bgColor.r, bgColor.g, bgColor.b, bgColor.a });
    ImGui::PopID();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    return result;
  }
  
  bool PropertyGrid::Float1(const char* label,
                            float& value,
                            bool* checkbox_flag,
                            float delta,
                            float reset_value,
                            float column_width,
                            float min_value,
                            float max_value) {
    return FloatImpl({"X"}, label, {&value}, checkbox_flag, delta, reset_value, column_width, min_value, max_value);
  }

  bool PropertyGrid::Float2(const char* label,
                            glm::vec2& value,
                            bool* checkbox_flag,
                            float delta,
                            float reset_value,
                            float column_width,
                            float min_value,
                            float max_value) {
    return FloatImpl({"X", "Y"}, label, {&value.x, &value.y}, checkbox_flag, delta, reset_value, column_width, min_value, max_value);
  }

  bool PropertyGrid::Float3(const char* label,
                            glm::vec3& value,
                            bool* checkbox_flag,
                            float delta,
                            float reset_value,
                            float column_width,
                            float min_value,
                            float max_value) {
    return FloatImpl({"X", "Y", "Z"}, label, {&value.x, &value.y, &value.z}, checkbox_flag, delta, reset_value, column_width, min_value, max_value);
  }
  
  bool PropertyGrid::FloatImpl(const std::vector<std::string>& buttons,
                               const char* label,
                               const std::vector<float*>& values,
                               bool* checkbox_flag,
                               float delta,
                               float reset_value,
                               float column_width,
                               float min_value,
                               float max_value) {
    bool modified = false;
    ImGuiIO& io   = ImGui::GetIO();
    auto bold_font = io.Fonts->Fonts[0];

    ImGui::PushID(label);
    ImGui::Columns(2);

    ImGui::PushItemWidth(-1);
    ImGui::SetColumnWidth(0, column_width);
    
    // this flag is to check we need to have slider float or not
    bool prop_flag;
    if (checkbox_flag) {
      ImGui::Checkbox(label, checkbox_flag);
      prop_flag = *checkbox_flag;
    }
    else {
      ImGui::Text(label);
      prop_flag = true;
    }
    ImGui::PopItemWidth();

    if (prop_flag) {
      ImGui::NextColumn();
      ImGui::PushItemWidth(-1);

      float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
      ImVec2 button_size = { lineHeight + 3.0f, lineHeight };
      
      ImGui::PushMultiItemsWidths((uint32_t)buttons.size(), ImGui::CalcItemWidth() - (lineHeight * buttons.size()));
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
      
      for (int32_t i = 0; i < buttons.size(); i++) {
        ImGui::PushFont(bold_font);
        if (ImGui::Button((const char*)&buttons[i], button_size)) {
          *(values[i]) = reset_value;
          modified = true;
        }
        ImGui::PopFont();
        
        ImGui::SameLine();
        std::string id = std::string("##") + std::to_string(i);
        if (ImGui::DragFloat(id.c_str(), values[i], delta, min_value, max_value, "%.2f"))
          modified = true;

        if (i < buttons.size() - 1)
          ImGui::SameLine();
      }
      ImGui::PopItemWidth();
      ImGui::PopStyleVar();
    }
    
    ImGui::Columns(1);
    ImGui::PopID();
    
    return modified;
  }

  bool PropertyGrid::TextBox(std::string& value,
                             const char* label,
                             uint32_t num_columns,
                             float column_width_1,
                             const char* hint,
                             bool modifiable,
                             bool multiple,
                             int32_t num_lines,
                             bool error) {
    IK_CORE_ASSERT(num_columns >= 2, "Column should be minimum 2");
    bool modified = false;
    std::string ui_context_id = "##";

    float x = ImGui::GetContentRegionAvailWidth();

    ImGui::Columns(num_columns);
    if (label and strcmp(label, "") != 0) {
      ImGui::PushID(label);
      ImGui::PushItemWidth(-1);
      ImGui::Text(label);
      ImGui::PopItemWidth();
      
      ui_context_id += (std::string)label;
    } else {
      ImGui::PushID("No Lable Text box");
      column_width_1 = 0;
    }

    ImGui::SetColumnWidth(0, column_width_1);
    
    if (hint) {
      ImGui::SameLine();
      HelpMarker(hint);
    }
    
    ImGui::NextColumn();
    
    ImGui::PushItemWidth(-1);
    float column_width_2 = x - column_width_1 - 16.0f;
    ImGui::SetColumnWidth(1, column_width_2);
    
    // Copy the Name of entity to buffer that will be dumy text in property pannel
    char buffer[256];
    strcpy(buffer, value.c_str());
        
    // To make string Red in case error flag is true
    if (error)
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    
    if (modifiable) {
      // Take input text from User in Property pannel. that will be name(Tag) of Selected Entity
      if (multiple) {
        if (ImGui::InputTextEx(ui_context_id.c_str(),
                               hint,
                               buffer,
                               IM_ARRAYSIZE(buffer),
                               ImVec2(column_width_2, num_lines * 20.0f),
                               ImGuiInputTextFlags_Multiline)) {
          value    = buffer;
          modified = true;
        }
      }
      else {
        if (ImGui::InputTextWithHint(ui_context_id.c_str(),
                                     hint,
                                     buffer,
                                     IM_ARRAYSIZE(buffer),
                                     ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
          value    = buffer;
          modified = true;
        }
      }
    }
    else
      ImGui::InputText(ui_context_id.c_str(),
                       (char*)value.c_str(),
                       256,
                       ImGuiInputTextFlags_ReadOnly);
    
    // Pop red color if error is enabled
    if (error)
      ImGui::PopStyleColor();
    
    ImGui::PopItemWidth();
    ImGui::PopID();
    
    if (num_columns == 2)
      ImGui::Columns(1);
    
    return modified;
  }
  
  uint32_t PropertyGrid::ComboDrop(const char* label,
                                   const std::vector<std::string>& options,
                                   uint32_t current_value,
                                   float col_width_1,
                                   float col_width_2) {
    uint32_t result = current_value;
    
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, col_width_1);
    ImGui::Text(label);
    
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::SetColumnWidth(1, col_width_2);
    
    std::string ui_context_id = "##" + (std::string)label;
    
    const char* current_type = options[(size_t)current_value].c_str();
    
    if (ImGui::BeginCombo(ui_context_id.c_str(), current_type)) {
      for (size_t i = 0; i < options.size(); i++) {
        bool is_selected = current_type == options[i];
        
        if (ImGui::Selectable(options[i].c_str(), is_selected)) {
          current_type = options[i].c_str();
          result = (uint32_t)i;
        }
        
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    
    ImGui::Columns(1);
    ImGui::Separator();
    
    return result;
  }
  
  bool PropertyGrid::CheckBox(const char* label,
                              bool& value,
                              float column_width_1,
                              float column_width_2) {
    bool modified = false;
    
    ImGui::Columns(2);
    ImGui::PushItemWidth(-1);
    ImGui::SetColumnWidth(0, column_width_1);
    ImGui::Text(label);
    ImGui::PopItemWidth();
    
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);
    ImGui::SetColumnWidth(1, column_width_2);
    std::string UIContextId = "##" + (std::string)label;
    
    if (ImGui::Checkbox(UIContextId.c_str(), &value))
      modified = true;
    
    ImGui::PopItemWidth();
    ImGui::NextColumn();
    ImGui::Columns(1);
    
    return modified;
  }
  
}
