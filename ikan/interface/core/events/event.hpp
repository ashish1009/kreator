//
//  event.hpp
//  ikan
//
//  Created by Ashish . on 25/12/22.
//

#pragma once

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
  
  /// This is the base class of all type of events.
  /// - Event is registered in window event callback so any event type should be deriived from this base class.
  /// - Event can be dispatchjed using even Dispatcher
  class Event {
  public:
    /// Type of Event supported by iKan
    enum class Type : uint8_t {
      NoEventType,
      WindowClose, WindowResize, WindowFocused, WindowHovered,
      KeyPressed, KeyReleased, KeyTyped,
      MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };
    
    /// Catergosy bits of Events
    enum Category : uint8_t {
      NoEventCategory = 0,
      EventCategoryApplication    = BIT(0),
      EventCategoryInput          = BIT(1),
      EventCategoryKeyboard       = BIT(2),
      EventCategoryMouse          = BIT(3),
      EventCategoryMouseButton    = BIT(4)
    };
    
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
    /// - Parameter - Event category type:
    bool IsInCategory(Category category) const;
    
  public:
    Event() = default;
    
    bool handled_ = false;
  };
  
  /// Class to dispatch event dynamically. Event type can be checked with current event and
  /// perform the functionality passed as funciton pointer
  class EventDispatcher {
  public:
    /// Constructor that store the event reference in the dispatcher
    ///  - Parameter event: Event reference triggered from window callback:
    EventDispatcher(Event& event);
    
    /// This function Dispatches the event dynamically
    ///  - Once an event callback triggered, it check the triggered event's type( with base calss 'Even')
    ///    with the explicit event type 'T'. If same then executes  the Function F (Function pointer passed to Dispatcher)
    ///  - Important: Event Should be Dispached using this function only. All Event handler takes only base
    /// Instance of Event. This Dispatcher Dispatches the function binded to a specific Event. So
    /// checked all the triggered events at same time and return the calling on Function Binded as
    /// Funciton pointer <F>
    ///  - Parameter func: Function pointer to be triggered when even dispatches:
    template<typename T, typename F> bool Dispatch(const F& func) {
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
  
}
