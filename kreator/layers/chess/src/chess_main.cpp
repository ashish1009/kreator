//
//  chess_main.cpp
//  chess
//
//  Created by Ashish . on 02/01/23.
//

#include "chess_main.hpp"

namespace chess {
  
  ChessLayer::ChessLayer() : Layer("Kreator") {
    IK_INFO("Chess", "Creating Chess Layer instance ... ");
    
    // Reinitialize the Batch Renderer
    BatchRenderer::Reinit(200, 0, 0);
    
#if CHESS_DEBUG
    spm_.SetSceneContext(&chess_scene_);
#endif
  }
  
  ChessLayer::~ChessLayer() {
    IK_WARN("Chess", "Destroying Chess Layer instance !!! ");
  }
  
  void ChessLayer::Attach() {
    IK_INFO("Chess", "Attaching Chess Layer instance");
    // ---------------------------------------------------------
    // Set the scene as playing
    // ---------------------------------------------------------
    chess_scene_.PlayScene();

    // ---------------------------------------------------------
    // Create the camera entity
    // ---------------------------------------------------------
    camera_entity_ = chess_scene_.CreateEntity();
    camera_entity_.GetComponent<TransformComponent>().translation.x = (BlockSize * (MaxCols - 1)) / 2;
    camera_entity_.GetComponent<TransformComponent>().translation.y = (BlockSize * (MaxRows - 1)) / 2;
    
    auto& camera_comp = camera_entity_.AddComponent<CameraComponent>();
    camera_comp.is_primary = true;
    camera_comp.is_fixed_aspect_ratio = true;
    
    // ----------------------------------------------------
    // Background Block Entity (Board Outline)
    // ----------------------------------------------------
    background_entity_ = chess_scene_.CreateEntity("Background");
    background_entity_.GetComponent<TransformComponent>().translation = {(BlockSize * (MaxCols - 1)) / 2, (BlockSize * (MaxRows - 1)) / 2, -0.2f};
    background_entity_.GetComponent<TransformComponent>().scale = {BlockSize * (MaxCols + 1), BlockSize * (MaxRows + 1), 1};
    
    {
      auto& quad_comp = background_entity_.AddComponent<QuadComponent>();
      quad_comp.texture_comp.use = true;
      quad_comp.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/common/background.png"));
    }

    // -------------------------------------------------------------------
    // Block Hint Entities (Hovered Selected and Possible move outline)
    // -------------------------------------------------------------------
    hovered_block_entity_ = chess_scene_.CreateEntity("Hovered Block");
    hovered_block_entity_.GetComponent<TransformComponent>().scale = {BlockSize, BlockSize, 1};
    {
      auto& quad_comp = hovered_block_entity_.AddComponent<QuadComponent>();
      quad_comp.texture_comp.use = true;
      quad_comp.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/common/hovered_block.png"));
    }

    selected_block_entity_ = chess_scene_.CreateEntity("Selected Block");
    selected_block_entity_.GetComponent<TransformComponent>().scale = {BlockSize, BlockSize, 1};
    {
      auto& quad_comp = selected_block_entity_.AddComponent<QuadComponent>();
      quad_comp.texture_comp.use = true;
      quad_comp.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/common/selected_block.png"));
    }

    // ----------------------------------------------------
    // Create Blocks
    // ----------------------------------------------------
    for (uint32_t row = 0; row < MaxRows; row++ ) { // Rows
      for (uint32_t col = 0; col < MaxCols; col++ ) { // Cols
        // Update the position of block
        // ------------------------------
        block_[row][col].row = row;
        block_[row][col].col = col;
        
        // Create Chess Block Entity (Black or white)
        // ------------------------------------------------
        auto b_e = chess_scene_.CreateEntity("Block_" + std::to_string(row) + "_" + std::to_string(col));
        
        // Create Quad component and update the color alternate
        auto& quad_comp = b_e.AddComponent<QuadComponent>();
        if (row % 2)
          quad_comp.color = (col % 2) ? BlackColor : WhiteColor;
        else
          quad_comp.color = (col % 2) ? WhiteColor : BlackColor;
        
        // Update the position of block
        b_e.GetComponent<TransformComponent>().translation = { row * BlockSize, col * BlockSize, 0 };
        b_e.GetComponent<TransformComponent>().scale = { BlockSize, BlockSize, 0 };
        
        // Add the piece to the block
        // ------------------------------
        block_[row][col].piece = Piece::Create(row, col);
        
        // ----------------------------------------------------
        // Create Piece Entities
        // ----------------------------------------------------
        if (block_[row][col].piece) {
          std::shared_ptr<Piece> piece = block_[row][col].piece;
          std::string piece_name = piece->GetColorStr() + "_" + piece->GetName();
          Entity p_e = chess_scene_.CreateEntity(piece_name);
          
          auto& quad_comp = p_e.AddComponent<QuadComponent>();
          p_e.GetComponent<TransformComponent>().translation = { piece->Col() * BlockSize, piece->Row() * BlockSize, 0.1f };
          p_e.GetComponent<TransformComponent>().scale = { BlockSize / 2, BlockSize / 2, 0 };

          quad_comp.texture_comp.use = true;
          quad_comp.texture_comp.component = piece->GetTexture();

          // ----------------------------------------------------
          // Hack to flip knight image
          // ----------------------------------------------------
          if (piece->GetType() == Piece::Type::Knight) {
            static int32_t knight_count = 0;
            if (knight_count++ %2 == 0) {
              p_e.GetComponent<TransformComponent>().scale.x *= -1.0f;
            }
          }
        } // if (block_[row][col].piece)
      } // Rows loop
    } // Cols loop
  }
  
  void ChessLayer::Detach() {
    IK_WARN("Chess", "Detaching Chess Layer instance ");
  }
  
  void ChessLayer::Update(Timestep ts) {
    if (viewport_.IsFramebufferResized()) {
      viewport_.framebuffer->Resize(viewport_.width, viewport_.height);
      // TODO: Store the player position before resize and back it up after resize
      chess_scene_.SetViewport(viewport_.width, viewport_.height);
    }
    
    viewport_.UpdateMousePos();
    viewport_.framebuffer->Bind();
    
    Renderer::Clear({0.12f, 0.12f, 0.12f, 1.0f});
    chess_scene_.Update(ts);

#if CHESS_DEBUG
    viewport_.UpdateHoveredEntity(spm_.GetSelectedEntity(), &chess_scene_);
#else
    viewport_.UpdateHoveredEntity(nullptr, &chess_scene_);
#endif
    
    // ----------------------------------------------------
    // Update the Hovered Block
    // ----------------------------------------------------
    {
      if (viewport_.hovered_entity_ and
          *viewport_.hovered_entity_ != background_entity_ and
          *viewport_.hovered_entity_ != hovered_block_entity_) {
        const auto& position = viewport_.hovered_entity_->GetComponent<TransformComponent>().translation;
        hovered_block_entity_.GetComponent<TransformComponent>().translation = { position.x, position.y, 0.3 };
      }
    }
    viewport_.framebuffer->Unbind();
  }
    
  void ChessLayer::EventHandler(Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(ChessLayer::OnWindowResized));
    dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(ChessLayer::OnMouseButtonPressed));
  }
  
  bool ChessLayer::OnWindowResized(WindowResizeEvent &e) {
    viewport_width_ = e.GetWidth();
    viewport_height_ = e.GetHeight();
    
    // TODO: Store the player position before resize and back it up after resize
    chess_scene_.SetViewport(viewport_width_, viewport_height_);
    return false;
  }
  
  bool ChessLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
    if (e.GetMouseButton() == MouseButton::ButtonLeft) {
      if (viewport_.mouse_pos_x >= 0 and
          viewport_.mouse_pos_y >= 0 and
          viewport_.mouse_pos_x <= viewport_.width and
          viewport_.mouse_pos_y <= viewport_.height) {
        
#if CHESS_DEBUG
        spm_.SetSelectedEntity(viewport_.hovered_entity_);
#endif
        
        // ----------------------------------------------------
        // Mouse Click Validations
        // ----------------------------------------------------
        if (viewport_.hovered_entity_) {
          // ------------------------------------------------------------------
          // For background we do not need click feature for chess
          // ------------------------------------------------------------------
          if (*viewport_.hovered_entity_ == background_entity_) {
            selected_block_ = nullptr;
            return false;
          }
          
          // ------------------------------------------------------------------
          // If Hovered entity is inside the chess board then update the
          // Selected Block pointer
          // ------------------------------------------------------------------
          const auto& position = viewport_.hovered_entity_->GetComponent<TransformComponent>().translation;
          uint32_t row = (uint32_t)(position.y / BlockSize);
          uint32_t col = (uint32_t)(position.x / BlockSize);
          
          if (row >= 0 and row < MaxRows and col >= 0 and col < MaxCols) {
            // Update the Selected Block Pointer
            // ---------------------------------
            selected_block_ = &block_[row][col];
            
            if (selected_block_->piece) {
              // -----------------------------------------------------------------
              // if selected block is not empty.
              //  - Render the Selected Block outline
              //  - Validate the piece
              // -----------------------------------------------------------------
              // TODO: Later do other validation??
              auto& selected_pos = selected_block_entity_.GetComponent<TransformComponent>().translation;
              
              if (position.x == selected_pos.x and position.y == selected_pos.y) {
                // ---------------------------------------------------------------
                // If We click same block again then reset the selected block by
                // moving the selected block outline entity in background.
                // Also Update the selected piece
                // ---------------------------------------------------------------
                selected_pos = {0, 0, -0.5}; // Move to background NOT VISIBLE
                selected_piece_ = nullptr;
              } else {
                // ---------------------------------------------------------------
                // If this click is new block then Update the position of the
                // selected block entity position same as the selected block position
                // ---------------------------------------------------------------
                selected_pos = {position.x, position.y, 0.25};
                selected_piece_ = selected_block_->piece.get();
              }

              // ---------------------------------------------------------------
              // Delete the entity of possible move. Everytime we click the new
              // block or same block. Clear the possible move block outline entities
              // ---------------------------------------------------------------
              ClearPossibleMoveEntity();

              // ---------------------------------------------------------------
              // Render Possible Move outlines
              // ---------------------------------------------------------------
              if (selected_piece_) {
                PossibleMoves moves = selected_piece_->GetPossibleMovePositions();
                
                // Validate the moves and Create entity for possible moves
                ValidatePossibleMoved(moves.up);
                ValidatePossibleMoved(moves.down);
                ValidatePossibleMoved(moves.right);
                ValidatePossibleMoved(moves.left);
                ValidatePossibleMoved(moves.up_right);
                ValidatePossibleMoved(moves.up_left);
                ValidatePossibleMoved(moves.down_left);
                ValidatePossibleMoved(moves.down_right);
              } else { // if (selected_piece_)
                // TODO: ??
              } // if (selected_piece_)
            } // if (selected_block_->piece)
          } // if (row >= 0 and row < MaxRows and col >= 0 and col < MaxCols)
          else {
            selected_block_ = nullptr;
          }
        } else { // if (viewport_.hovered_entity_)
          selected_block_ = nullptr;
        }
      }
    }
    return false;
  }
  
  void ChessLayer::ValidatePossibleMoved(const PossiblePositions& positions) {
    // Validate empty blocks
    int32_t idx = 0;
    for (const auto& [row, col] : positions) {
      if (block_[row][col].piece) {
        if (block_[row][col].piece->GetColor() != selected_piece_->GetColor()) {
          CreatePossibleMoveEntity(row, col);
        }
        break;
      } else {
        if (selected_piece_->ValidateIfPawn(col))
          CreatePossibleMoveEntity(row, col);
      }
      idx++;
    } // for (const auto& [row, col] : moves.empty_blocks_)
  }
  
  void ChessLayer::CreatePossibleMoveEntity(Position row, Position col) {
    Entity e = chess_scene_.CreateEntity("Possible_" + std::to_string(row) + "_" + std::to_string(col));
    e.GetComponent<TransformComponent>().translation = {col, row, 0.3f};
    e.GetComponent<TransformComponent>().scale = {BlockSize, BlockSize, 1.0f};
    
    auto& quad_comp = e.AddComponent<QuadComponent>();
    quad_comp.texture_comp.use = true;
    quad_comp.texture_comp.component = Renderer::GetTexture(AM::ClientAsset("textures/common/possible_move.png"));
    possible_move_entities_.push_back(e);
  }
  
  void ChessLayer::ClearPossibleMoveEntity() {
    if (possible_move_entities_.size() > 0) {
      for (auto& entity : possible_move_entities_) {
        chess_scene_.DestroyEntity(entity);
      }
      possible_move_entities_.clear();
    }
  }
  
  void ChessLayer::RenderGui() {
    ImguiAPI::StartDcocking();

#if CHESS_DEBUG
    Renderer::RenderStatsGui(nullptr, true);
    Renderer::Framerate();
    
    chess_scene_.RenderGui();
        
    spm_.RenderGui();
    viewport_.RenderGui();

    ImGui::Begin("Block Data");
    float width = ImGui::GetContentRegionAvailWidth() / 2;
    ImGui::SetCursorPos({ width - 50, 0 });
    ImGui::Text("Selected Block");
    ImGui::Separator();
    if (selected_block_) {
      ImGui::PushID("Selected Block");
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, width);
      ImGui::Text("Position");
      ImGui::Text("Piece");
      
      ImGui::NextColumn();
      ImGui::Text("Row : %d | Col : %d", selected_block_->row, selected_block_->col);
      if (selected_block_->piece)
        ImGui::Text("%s", selected_block_->piece->GetName().c_str());
      else
        ImGui::Text("Empty");
      ImGui::Columns(1);
      ImGui::PopID();
    }
    ImGui::End();


    ImGui::Begin("Selected Piece");
    width = ImGui::GetContentRegionAvailWidth() / 2;
    ImGui::SetCursorPos({ width - 50, 0 });
    ImGui::Text("Selected Piece");
    ImGui::Separator();
    if (selected_piece_) {
      ImGui::PushID("Selected Piece");
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, width);
      ImGui::Text("Name");
      ImGui::Text("Position");
      ImGui::Text("Color");
      
      ImGui::NextColumn();
      ImGui::Text("%s", selected_piece_->GetName().c_str());
      ImGui::Text("Row : %d | Col : %d", selected_piece_->Row(), selected_piece_->Col());
      ImGui::Text("%s", selected_piece_->GetColorStr().c_str());
      ImGui::Columns(1);
      ImGui::PopID();
    }
    ImGui::End();

#endif

    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Chess");
    ImGui::PushID("Chess");
    
    viewport_.focused = ImGui::IsWindowFocused();
    viewport_.hovered = ImGui::IsWindowHovered();
    
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    viewport_.width = viewport_panel_size.x;
    viewport_.height = viewport_panel_size.y;
    
    size_t textureID = viewport_.framebuffer->GetColorAttachmentIds().at(0);
    ImGui::Image((void*)textureID,
                 viewport_panel_size,
                 ImVec2{ 0, 1 },
                 ImVec2{ 1, 0 });
    
    viewport_.UpdateBound();
    
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();
    
    ImguiAPI::EndDcocking();
  }

}
