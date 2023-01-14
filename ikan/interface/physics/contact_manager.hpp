//
//  contact_manager.hpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#pragma once

#include "broad_phase.hpp"

namespace physics {
  
  class Contact;
  class ContactFilter;
  class ContactListener;
  class BlockAllocator;
  
  // Delegate of World.
  class ContactManager {
  public:
    ContactManager();
    
    // Broad-phase callback.
    void AddPair(void* proxyUserDataA, void* proxyUserDataB);
    
    void FindNewContacts();
    
    void Destroy(Contact* c);
    
    void Collide();
    
    BroadPhase broad_phase_;
    Contact* contact_list_;
    int32_t contact_count_;
    ContactFilter* contact_filter_;
    ContactListener* contact_listener_;
    BlockAllocator* allocator_;
  };
    
}
