//
//  event.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

// This file includes the base class of all events. This Class is interface for
// any even triggering in the Window

namespace ikan {
  
#define EVENT_CLASS_TYPE(type) \
static Event::Type GetStaticType() { \
return Event::Type::type; \
} \
virtual Event::Type GetEventType() const override { \
return GetStaticType(); \
} \
virtual const char* GetName() const override { \
return #type; \
}
  
#define EVENT_CLASS_CATEGORY(category) \
virtual int32_t GetCategoryFlags() const override { \
return category; \
}
  
// API to bind class member function to std function pointer
#define IK_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
  
  /// This is the base class of all type of events. This class type is regustered in window event
  /// callback so any event type should be deriived from this base class. Event can be dispatchjed
  /// using even Dispatcher
  class Event {
  public:
    /// Type of Event supported by iKan
    enum class Type : uint8_t {
      kNoEventType,
      kWindowClose, kWindowResize, kWindowFocused, kWindowHovered,
      kKeyPressed, kKeyReleased, kKeyTyped,
      kMouseButtonPressed, kMouseButtonReleased, kMouseMoved, kMouseScrolled
    };
    
    /// Event Categorisation
    enum Category : uint8_t {
      kNoEventCategory = 0,
      kEventCategoryApplication    = BIT(0),
      kEventCategoryInput          = BIT(1),
      kEventCategoryKeyboard       = BIT(2),
      kEventCategoryMouse          = BIT(3),
      kEventCategoryMouseButton    = BIT(4)
    };
    
    /// Default Event abstract destrutor
    virtual ~Event() = default;
    
    /// This function returns the type of triggered Event. e.g. WindowClosed...
    virtual Type GetEventType() const = 0;
    /// This function returns the Name of Event in char (Convert the type of Event in const char)
    virtual const char* GetName() const = 0;
    /// This function returns the Caregory Flag of Event as Bit poisition defined in enum Category
    virtual int32_t GetCategoryFlags() const = 0;
    /// This function prints the event and its data in readable way using 'IK_LOG' API
    virtual void Print() const = 0;
    
    /// This function checks if Event is aa same category as argument 'category'
    bool IsInCategory(Category category) const;
    
  public:
    bool handled_ = false;
  };
  
  /// Class to dispatch event dynamically. Event type can be checked with current event and
  /// perform the functionality passed as funciton pointer
  class EventDispatcher {
  public:
    /// Constructor that store the event reference in the dispatcher
    /// - Parameter event: Event
    EventDispatcher(Event& event);
    
    template<typename T, typename F>
    /// This function Dispatches the event. Once an event callback triggered, it check the triggered
    /// event's type( with base calss 'Even') with the explicit event type 'T'. If same then executes  the
    /// Function F (Function pointer passed to Dispatcher)
    /// NOTE : Template -   <T> - Type of Dispatched event
    ///               - <F> - Function type. This is dynmically executed.
    ///                         It stores the type of function passed as pointer
    /// - Parameter func: Function pointer that need to dispatch on event occurance
    /// ------------------------------------------------------------------
    /// Event Should be Dispached using this function only. All Event handler takes only base
    /// Instance of Event. This Dispatcher Dispatch the function binded to a specific Event. So
    /// checked all the triggered events at same time and return the calling on Function Binded as
    /// Funciton pointer <F>
    /// ------------------------------------------------------------------
    bool Dispatch(const F& func) {
      if (event_.GetEventType() == T::GetStaticType()) {
        event_.handled_ |= func(static_cast<T&>(event_));
        return true;
      }
      return false;
    }
    
  private:
    Event& event_;
  };
  
  // Using Typedefs
  using EventCallbackFn = std::function<void(Event&)>;
  
} // namespace ikan
