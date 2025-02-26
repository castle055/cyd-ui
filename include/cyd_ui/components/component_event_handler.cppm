// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "component_event_macros.h"

export module cydui.components.event_handler;

export import std;

export import cydui.components.base;
export import cydui.components.anchors;

export import cydui.events;
export import cydui.graphics;


#define TO_STRING(...) #__VA_ARGS__
#define ANCHOR(PREFIX, NAME) \
  struct NAME { \
    static constexpr dimension_parameter_t x{TO_STRING(PREFIX##_##NAME##_x)}; \
    static constexpr dimension_parameter_t y{TO_STRING(PREFIX##_##NAME##_y)}; \
  }

#define CYDUI_INTERNAL_EV_HANDLER_DECL(NAME) void on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS

#define CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(NAME)                                                 \
CYDUI_INTERNAL_EV_##NAME##_RETURN on_##NAME CYDUI_INTERNAL_EV_##NAME##_ARGS


export {
  namespace cyd::ui::components {


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

    //! EVENT HANDLER STRUCT
    struct event_handler_t {
    private:
      component_base_t* component = nullptr;

    public:
      const std::list<std::shared_ptr<component_base_t>>& $children;
      static constexpr bool handles_text_input = false;

      event_handler_t(component_base_t *comp, const std::list<std::shared_ptr<component_base_t> > &$children_)
        : component(comp),
          $children($children_) {
      }

      // virtual ~event_handler_t() {}

      CYDUI_INTERNAL_EV_HANDLER_DECL_W_RET(redraw) {
        return {};
      }

#define DIMENSIONAL_ARGS                                                                           \
  $x, $y, $width, $height, $padding_top, $padding_bottom, $padding_left, $padding_right

      // ? MOUSE EVENTS
#define CYDUI_INTERNAL_EV_button_PROPAGATE(NAME)                                                   \
  if (component->parent.has_value()) \
    component->parent.value()->get_event_dispatcher()->dispatch_button_##NAME(button, x + $x, y + $y);
#define CYDUI_INTERNAL_EV_mouse_PROPAGATE(NAME)                                                    \
  if (component->parent.has_value()) \
    component->parent.value()->get_event_dispatcher()->dispatch_mouse_##NAME(x + $x, y + $y);

      // * button press
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_press) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(press);
      }
      // * button release
      CYDUI_INTERNAL_EV_HANDLER_DECL(button_release) {
        CYDUI_INTERNAL_EV_button_PROPAGATE(release);
      }
      // * mouse enter
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_enter) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(enter);
      }
      // * mouse exit
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_exit) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(exit);
      }
      // * mouse motion
      CYDUI_INTERNAL_EV_HANDLER_DECL(mouse_motion) {
        CYDUI_INTERNAL_EV_mouse_PROPAGATE(motion);
      }
      // * mouse scroll
      CYDUI_INTERNAL_EV_HANDLER_DECL(scroll) {
        if (component->parent.has_value())
          component->parent.value()->get_event_dispatcher()->dispatch_scroll(dx, dy);
      }

      // ? KEYBOARD EVENTS
      // * key press
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_press) {
        if (component->parent.has_value())
          component->parent.value()->get_event_dispatcher()->dispatch_key_press(ev);
      }
      // * key release
      CYDUI_INTERNAL_EV_HANDLER_DECL(key_release) {
        if (component->parent.has_value())
          component->parent.value()->get_event_dispatcher()->dispatch_key_release(ev);
      }

      // * text input
      CYDUI_INTERNAL_EV_HANDLER_DECL(text_input) {
      }

      void draw_fragment CYDUI_INTERNAL_EV_fragment_ARGS {}
    };

#pragma clang diagnostic pop

    template <typename Component>
    struct event_handler_data_t: public event_handler_t {
      event_handler_data_t(
        Component&                                          $component_,
        const std::list<std::shared_ptr<component_base_t>>& $children_,
        const std::shared_ptr<typename Component::state_t>& state_,
        const std::shared_ptr<fabric::async::async_bus_t>&  window_,
        typename Component::props_t&                        props_,
        attrs_component<Component>&                         attrs_,
        typename Component::style_t&                        style_
      )
          : event_handler_t(&$component_, $children_),
            $component($component_),
            state(*state_),
            window(*window_),
            props(props_),
            attrs(attrs_),
            style(style_) {}

      Component&                   $component;
      typename Component::state_t& state;
      fabric::async::async_bus_t&  window;
      typename Component::props_t& props;
      attrs_component<Component>&  attrs;
      typename Component::style_t& style;

      using $self = anchors::self_component;
      using $parent = anchors::parent_component;
      using $previous = anchors::previous_component;
    };
  } // namespace cyd::ui::components

  template <typename Event>
  class custom_event_listener {
  public:
    custom_event_listener(fabric::async::async_bus_t*  window, std::function<void(const Event&)> callback, std::function<void()> post): window_(window), callback_(callback), post_(post) {
      start_listening();
    }

    ~custom_event_listener() {
      stop_listening();
    }

    custom_event_listener(const custom_event_listener& other): window_(other.window_), callback_(other.callback_), post_(other.post_) {
      start_listening();
    }
    custom_event_listener& operator=(const custom_event_listener& other) {
      stop_listening();
      this->window_ = other.window_;
      this->callback_ = other.callback_;
      this->post_ = other.post_;
      start_listening();
      return *this;
    }

    custom_event_listener(custom_event_listener&& other) noexcept: window_(other.window_), callback_(other.callback_), post_(other.post_) {
      other.stop_listening();
      start_listening();
    }
    custom_event_listener& operator=(custom_event_listener&& other) {
      stop_listening();
      this->window_ = other.window_;
      this->callback_ = other.callback_;
      this->post_ = other.post_;
      other.stop_listening();
      start_listening();
      return *this;
    }
  private:
    void start_listening() {
      stop_listening();

      listener_ = window_->on_event([&](const Event& ev) {
        callback_(ev);
        post_();
      });
    }
    void stop_listening() {
      if (listener_.has_value()) {
        listener_.value().remove();
        listener_ = std::nullopt;
      }
    }
  private:
    fabric::async::async_bus_t*  window_;
    std::function<void(const Event&)> callback_;
    std::function<void()> post_;
    std::optional<fabric::async::listener<Event>> listener_ {std::nullopt};
  };
}