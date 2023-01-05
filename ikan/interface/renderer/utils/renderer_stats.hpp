//
//  renderer_stats.hpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#pragma once

namespace ikan {
  
  /// Stores the statistics of renderer
  struct RendererStatistics {
    // ------------------
    // Member Variables
    // ------------------
    uint32_t vertex_buffer_size = 0, index_buffer_size = 0, texture_buffer_size = 0;
    
    uint32_t draw_calls    = 0;
    uint32_t vertex_count  = 0;
    uint32_t index_count   = 0;
    
    // ------------------
    // Member Functions
    // ------------------
    /// This function resets only those stats that need to be reset each frame
    void ResetEachFrame();
    /// This function resets all stats
    void ResetAll();
    /// This function shows the stats in gui widget
    /// - Parameters:
    ///   - is_open: flag to show or hide the widget
    ///   - summary: show stats in summary or detailed
    void RenderGui(bool *is_open = nullptr, bool summary = false);
    // ------------------
    // Static Functions
    // ------------------
    /// This function returns the static singleton instance of RendererStatistics
    static RendererStatistics& Get();
    
  private:
    RendererStatistics() = default;
    DELETE_COPY_MOVE_CONSTRUCTORS (RendererStatistics);
  };
  
} // namespace ikan
