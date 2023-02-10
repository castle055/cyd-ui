//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_HPP
#define CYD_UI_COMPONENTS_HPP

#include "children_state_collection.hpp"
#include "colors.hpp"
#include "dimensions.hpp"
#include "events.hpp"
#include "window_types.hpp"
#include "event_types.h"
#include <functional>
#include <optional>
#include <unordered_set>
#include <vector>

namespace cydui::components {
    class Component;
    
    
    class ComponentBorder {
    public:
      bool enabled = false;
      layout::color::Color* color = new layout::color::Color("#FCAE1E");
    };
    
    class ComponentState {
    public:
      ComponentState();
      
      Component* component_instance;
      bool stateless_comp = false;
      bool focused = false;
      
      bool _dirty = false;
      
      void dirty();
      
      graphics::window_t* win = nullptr;
      graphics::render_target_t* sub_render_target = nullptr;
      
      dimensions::component_dimensions_t dim;
      ComponentBorder border;
      ChildrenStateCollection children;
    };
    
    template<class c>
    concept ComponentConcept = requires {
      typename c::Props;
      typename c::State;
      {
      c::props
      } -> std::convertible_to<typename c::Props>;
    };
    
    typedef std::function<Component*()> component_builder_t;
    
    template<typename c> requires ComponentConcept<c>
    struct c_init_t {
      typename c::State** ref = nullptr;
      typename c::Props props;
      
      std::optional<dimensions::dimensional_relation_t> x;
      std::optional<dimensions::dimensional_relation_t> y;
      std::optional<dimensions::dimensional_relation_t> w;
      std::optional<dimensions::dimensional_relation_t> h;
      
      std::vector<component_builder_t> inner = {};
      
      std::function<void(c*)> init = [](c*) {
      };
    };
    
    
    class Component {
      
      std::function<void(Component*)> inner_redraw = [](Component*) {
      };
      
      bool is_group = false;
    
    protected:
      
      template<typename c, int ID>
      requires ComponentConcept<c>
      inline component_builder_t create(c_init_t<c> init) {
        auto* c_state = (typename c::State*) (this->state->children.contains(ID)
          ? (this->state->children[ID])
          : (this->state->children.add(ID, new typename c::State())));
        c_state->win = this->state->win;
        if (init.ref)
          *(init.ref) = c_state;
        return [this, init, c_state]() {
          auto* _c = new c(
            c_state,
            init.props,
            [init](cydui::components::Component* __raw_local_) {
              auto* local = (c*) __raw_local_;
              
              if (init.x.has_value())
                __raw_local_->state->dim.x = init.x.value();
              if (init.y.has_value())
                __raw_local_->state->dim.y = init.y.value();
              if (init.w.has_value()) {
                __raw_local_->state->dim.w = init.w.value();
                __raw_local_->state->dim.given_w = true;
              }
              if (init.h.has_value()) {
                __raw_local_->state->dim.h = init.h.value();
                __raw_local_->state->dim.given_h = true;
              }
              
              local->add(init.inner);
              init.init(local);
            });
          return _c;
        };
      }

#define COMP(COMPONENT) create<COMPONENT, __COUNTER__>
      
      template<typename c, int ID, typename T>
      requires ComponentConcept<c>
      inline component_builder_t create_for(
        T& iter, std::function<c_init_t<c>(typename T::value_type)> block
      ) {
        std::vector<ComponentState*> states = {};
        int k = 0;
        for (auto a = iter.begin(); a != iter.end(); ++a, ++k) {
          states.push_back(
            (typename c::State*) (this->state->children.contains(ID, k)
              ? (this->state->children.get_list(ID, k))
              : (this->state->children.add_list(ID, k, new typename c::State())))
          );
        }
        return [this, iter, block, states]() {
          int i = 0;
          auto temp_c = new Component();
          for (auto a = iter.begin(); a != iter.end(); ++a, ++i) {
            const c_init_t<c> init = block(*a);
            if (init.ref)
              *(init.ref) = nullptr;
            (states[i])->win = this->state->win;
            temp_c->children.push_back(
              new c((typename c::State*) states[i],
                init.props,
                [init](cydui::components::Component* __raw_local_) {
                  auto* local = (c*) __raw_local_;
                  
                  if (init.x.has_value())
                    __raw_local_->state->dim.x = init.x.value();
                  if (init.y.has_value())
                    __raw_local_->state->dim.y = init.y.value();
                  if (init.w.has_value()) {
                    __raw_local_->state->dim.w = init.w.value();
                    __raw_local_->state->dim.given_w = true;
                  }
                  if (init.h.has_value()) {
                    __raw_local_->state->dim.h = init.h.value();
                    __raw_local_->state->dim.given_h = true;
                  }
                  
                  local->add(init.inner);
                  init.init(local);
                }));
          }
          temp_c->is_group = true;
          return temp_c;
        };
      }

#define FOR_EACH(COMPONENT) create_for<COMPONENT, __COUNTER__>
#define NULLCOMP            [](){ return Component::new_group(); }
    
    
    public:
      static Component* new_group();
      //explicit Component(std::unordered_set<Component*> children);
      
      //explicit Component(ComponentState* state, std::unordered_set<Component*> children);
      
      Component();
      
      Component(std::function<void(Component*)> inner);
      
      Component(ComponentState* state);
      
      Component(ComponentState* state, std::function<void(Component*)> inner);
      
      virtual ~Component();
      
      Component* parent;
      ComponentState* state;
      cydui::dimensions::component_dimensions_t* dim = nullptr;
      
      void add(const std::vector<component_builder_t> &ichildren, bool prepend = false);
      
      std::deque<Component*> children;
      
      void redraw();
      
      void render(cydui::graphics::render_target_t* target);
      
      Component* get_parent();
      
      //Component* set_size(int w, int h);
      //
      //Component* set_size(IntProperty* w, IntProperty* h);
      //
      //Component* set_size(IntProperty::IntBinding w, IntProperty::IntBinding h);
      //
      //
      //Component* set_width(int w);
      
      //Component* set_width(IntProperty* w);
      
      //Component* set_width(IntProperty::IntBinding w);
      //
      //
      //Component* set_height(int h);
      
      //Component* set_height(IntProperty* h);
      
      //Component* set_height(IntProperty::IntBinding h);
      //
      //
      //Component* set_pos(Component* relative, int x, int y);
      
      //Component* set_pos(Component* relative, IntProperty* x, IntProperty* y);
      
      //Component* set_pos(
      //        Component* relative,
      //        IntProperty::IntBinding x,
      //        IntProperty::IntBinding y
      //);
      //
      //Component* set_padding(
      //        unsigned int top,
      //        unsigned int right,
      //        unsigned int bottom,
      //        unsigned int left
      //);
      //
      //Component* set_margin(
      //        unsigned int top,
      //        unsigned int right,
      //        unsigned int bottom,
      //        unsigned int left
      //);
      
      Component* set_border_enable(bool enabled);
      
      virtual void on_render(cydui::graphics::render_target_t* target);
      
      virtual void on_redraw();
      
      virtual void on_mouse_enter(int x, int y);
      
      virtual void on_mouse_exit(int x, int y);
      
      virtual void on_mouse_motion(int x, int y);
      
      virtual void on_mouse_click(int x, int y, int button);
      
      virtual void on_scroll(int d);
      
      typedef typename KeyEvent::DataType KeyData;
      
      virtual void on_key_press(KeyData key);
      
      virtual void on_key_release(KeyData key);
    };
  
}// namespace cydui::components


#define STATE(NAME) struct NAME##State: public cydui::components::ComponentState

#define INIT_STATE(NAME)                                                       \
  explicit NAME##State(): cydui::components::ComponentState()

#define COMPONENT(NAME)                                                        \
  using namespace primitives;                                                  \
  struct NAME: public cydui::components::Component

#define DISABLE_LOG this->log.on = false;
#define ENABLE_LOG  this->log.on = true;

#define PROPS(block)                                                           \
  struct Props block;                                                          \
  Props props;

#define NO_PROPS PROPS({})

#define INIT_W_STATE(NAME, STATE_CLASS)                                        \
  explicit NAME##Component(STATE_CLASS* state)                                 \
      : cydui::components::Component(state)

#define INIT(NAME)                                                             \
  typedef NAME##State State;                                                   \
  NAME##State* state  = nullptr;                                               \
  logging::logger log = {.name = #NAME, .on = false};                          \
  explicit NAME(NAME##State* state,                                            \
      Props props,                                                             \
      const std::function<void(cydui::components::Component*)>& inner)         \
      : cydui::components::Component(state, inner), props(std::move(props)),   \
        state(state)

#define REDRAW void on_redraw() override

#define RENDER(TARGET) void on_render(cydui::graphics::render_target_t*(TARGET)) override


#define WITH_STATE(NAME) auto* state = (NAME##State*)this->state;

#define ADD_TO(COMPONENT, VECTOR)                                              \
  COMPONENT->add(std::vector<cydui::components::Component*> VECTOR);

#define REDRAWw(NAME, VECTOR)                                                  \
  void on_redraw() override {                                                  \
    WITH_STATE(NAME)                                                           \
    ADD_TO(this, VECTOR);                                                      \
  }

/// Using lambda captures ___inner statements prevents IDE from showing 'unused-lambda-capture' error

// FIXME - REMOVE MACRO
#define ___inner(NAME, LOCAL_NAME, VECTOR)                                     \
  [state](cydui::components::Component* __raw_##LOCAL_NAME) {                  \
    state;                                                                     \
    auto* LOCAL_NAME = (NAME*)__raw_##LOCAL_NAME;                              \
    LOCAL_NAME->add(VECTOR);                                                   \
  }

#endif//CYD_UI_COMPONENTS_HPP
