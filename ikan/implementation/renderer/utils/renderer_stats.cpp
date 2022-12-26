//
//  renderer_stats.cpp
//  ikan
//
//  Created by Ashish . on 26/12/22.
//

#include "renderer_stats.hpp"

namespace ikan {
  
  void RendererStatistics::ResetEachFrame() {
    draw_calls = 0;
    index_count = 0;
    vertex_count = 0;
  }
  
  void RendererStatistics::ResetAll() {
    vertex_buffer_size = 0;
    index_buffer_size = 0;
    texture_buffer_size = 0;
    
    draw_calls = 0;
    index_count = 0;
    vertex_count = 0;
  }
  
  RendererStatistics& RendererStatistics::Get() {
    static RendererStatistics instance;
    return instance;
  }
  
}
