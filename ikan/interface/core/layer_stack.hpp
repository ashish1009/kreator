//
//  layer_stack.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes a class to store layers in a stack "Wrapper"

#include "core/layer.hpp"

namespace ikan {
  
  /// Stack to store all the Layers used by application Responsible for Insertion, deletion and
  /// Iteration of layers Storing data as std::vector
  class LayerStack {
  public:
    // ----------------------------
    // Constructors and Destructor
    // ----------------------------
    /// Default constructor of layer stack. Just creating isntance
    LayerStack();
    /// Destructor of layer stack. Remove all the layer from stack and detach
    /// them from application
    ~LayerStack();
    
    // -------------
    // Fundamentals
    // -------------
    /// This function pushes the layer in the Stack
    /// - Parameter layer: layer shaerd pointer to be inserted
    void PushLayer(const std::shared_ptr<Layer>& layer);
    
    /// This function erases the layer from the Stack
    /// - Parameter layer layer shared pointer to be deleted
    void PopLayer(const std::shared_ptr<Layer>& layer);
    
    // ---------------------
    // APIs for Iterators
    // ---------------------
    std::vector<std::shared_ptr<Layer>>::iterator begin();
    std::vector<std::shared_ptr<Layer>>::iterator end();
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin();
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rend();
    
    std::vector<std::shared_ptr<Layer>>::const_iterator begin() const;
    std::vector<std::shared_ptr<Layer>>::const_iterator end() const;
    std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const;
    std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rend() const;
    
    DELETE_COPY_MOVE_CONSTRUCTORS(LayerStack);
    
  private:
    uint32_t number_of_layers_ = 0;
    std::vector<std::shared_ptr<Layer>> layers_;
  };
  
} // namespace ikan
