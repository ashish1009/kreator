//
//  layer_stack.cpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#include "layer_stack.hpp"

namespace ikan {
  
  LayerStack::LayerStack() {
    IK_CORE_TRACE("Layer Stack", "Creating Layerstack ...");
  }
  
  LayerStack::~LayerStack() {
    IK_CORE_WARN("Layer Stack", "Destroying Layerstack !!!");
    IK_CORE_WARN("Layer Stack", "Deleting all Layers from Layerstack");
    
    // Delete and deteach all the layers from stack
    for (auto& layer : layers_) {
      IK_CORE_WARN("Layer Stack", "Poping the Layer in the stack");
      IK_CORE_WARN("Layer Stack", "  Name                            | {0}", layer->GetName().c_str());
      IK_CORE_WARN("Layer Stack", "  Number of Layers                | {0}", --number_of_layers_);
      
      layer->Detach();
    }
    
    layers_.erase(layers_.cbegin(), layers_.cend());
  }
  
  void LayerStack::PushLayer(const std::shared_ptr<Layer>& layer) {
    if (std::find(layers_.begin(), layers_.end(), layer) != layers_.end())
      return;
    
    IK_CORE_INFO("Layer Stack", "Pushing the Layer in the stack");
    IK_CORE_INFO("Layer Stack", "  Name             | {0}", layer->GetName().c_str());
    IK_CORE_INFO("Layer Stack", "  Number of Layers | {0}", ++number_of_layers_);
    
    layers_.emplace_back(layer);
    layer->Attach();
  }
  
  void LayerStack::PopLayer(const std::shared_ptr<Layer>& layer) {
    IK_CORE_WARN("Layer Stack", "Poping the Layer in the stack");
    IK_CORE_WARN("Layer Stack", "  Name                            | {0}", layer->GetName().c_str());
    IK_CORE_WARN("Layer Stack", "  Number of Layers                | {0}", --number_of_layers_);
    
    // Search the layer from the stack the detach/delete it
    if (auto it = std::find(layers_.begin(), layers_.end(), layer);
        it != layers_.end()) {
      layer->Detach();
      layers_.erase(it);
    }
  }
  
  std::vector<std::shared_ptr<Layer>>::iterator
  LayerStack::begin() {
    return layers_.begin();
  }
  
  std::vector<std::shared_ptr<Layer>>::iterator
  LayerStack::end() {
    return layers_.end();
  }
  
  std::vector<std::shared_ptr<Layer>>::reverse_iterator
  LayerStack::rbegin() {
    return layers_.rbegin();
  }
  
  std::vector<std::shared_ptr<Layer>>::reverse_iterator
  LayerStack::rend() {
    return layers_.rend();
  }
  
  std::vector<std::shared_ptr<Layer>>::const_iterator
  LayerStack::begin() const {
    return layers_.begin();
  }
  
  std::vector<std::shared_ptr<Layer>>::const_iterator
  LayerStack::end() const {
    return layers_.end();
  }
  
  std::vector<std::shared_ptr<Layer>>::const_reverse_iterator
  LayerStack::rbegin() const {
    return layers_.rbegin();
  }
  std::vector<std::shared_ptr<Layer>>::const_reverse_iterator
  LayerStack::rend() const {
    return layers_.rend();
  }
  
}
