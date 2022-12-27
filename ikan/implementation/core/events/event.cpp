//
//  event.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "event.hpp"

namespace ikan {
    
  bool Event::IsInCategory(Category category) const {
    return GetCategoryFlags() & category;
  }
  
  EventDispatcher::EventDispatcher(Event& event)
  : event_(event) {}
  
} 
