//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_HPP
#define CYD_UI_COMPONENTS_HPP

#include "event_types.h"
#include "types.hpp"

namespace cydui::components {
    class Component {
      
      std::function<void(Component*)> inner_redraw = [](Component*) {
      };
      
      bool is_group = false;
    
    private:
      template<typename c>
      requires ComponentConcept<c>
      static std::function<void(Component*)> get_init_function(c_init_t<c> init) {
        return [init](cydui::components::Component* __raw_local_) {
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
        };
      }
    
    protected:

#define SET_REFERENCE if (init.ref) *(init.ref) =
#define SET_WINDOW(STATE) (STATE)->win = this->state->win
#define INSTANTIATE_COMP(STATE) new c(STATE, init.props, get_init_function(init))
      
      template<typename c, int ID>
      requires ComponentConcept<c>
      inline component_builder_t create(c_init_t<c> init) {
        auto* c_state = (typename c::State*) (state->children.contains(ID)
          ? (state->children[ID])
          : (state->children.add(ID, new typename c::State())));
        SET_WINDOW(c_state);
        SET_REFERENCE c_state;
        return [this, init, c_state]() {
          auto* _c = INSTANTIATE_COMP(c_state);
          return _c;
        };
      }
      
      template<typename c, int ID, typename T>
      requires ComponentConcept<c>
      inline component_builder_t create_for(
        T &iter, std::function<c_init_t<c>(typename T::value_type)> block
      ) {
        std::vector<typename c::State*> states = {};
        int k = 0;
        for (auto a = iter.begin(); a != iter.end(); ++a, ++k) {
          states.push_back(
            (typename c::State*) (state->children.contains(ID, k)
              ? (state->children.get_list(ID, k))
              : (state->children.add_list(ID, k, new typename c::State())))
          );
        }
        return [this, iter, block, states]() {
          int i = 0;
          auto temp_c = Component::new_group();
          for (auto a = iter.begin(); a != iter.end(); ++a, ++i) {
            const c_init_t<c> init = block(*a);
            SET_REFERENCE nullptr;
            SET_WINDOW(states[i]);
            temp_c->children.push_back(
              INSTANTIATE_COMP(states[i])
            );
          }
          return temp_c;
        };
      }


#define NULLCOMP            [](){ return Component::new_group(); }

#define COMP(COMPONENT) create<COMPONENT, __COUNTER__>

#define IF(CONDITION) [&,this]()->component_builder_t { if (CONDITION) return
#define ELSE                                            else return
#define ELSE_IF(CONDITION)                              else if(CONDITION) return

#define WHEN(VARIABLE) [&,this]()->component_builder_t { auto value = VARIABLE;
#define CASE(VALUE)                 if (value == VALUE) return
#define END           return NULLCOMP; }()

#define FOR_EACH(COMPONENT) create_for<COMPONENT, __COUNTER__>
    
    
    public:
      static Component* new_group();
      
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

#define INIT(NAME)                                                             \
  typedef NAME##State State;                                                   \
  NAME##State* state  = nullptr;                                               \
  static constexpr const char* Name = #NAME;                                                    \
  logging::logger log = {.name = #NAME, .on = false};                          \
  explicit NAME(NAME##State* state,                                            \
      Props props,                                                             \
      const std::function<void(cydui::components::Component*)>& inner)         \
      : cydui::components::Component(state, inner), props(std::move(props)),   \
        state(state)

#define REDRAW void on_redraw() override

#define RENDER(TARGET) void on_render(cydui::graphics::render_target_t*(TARGET)) override

#endif//CYD_UI_COMPONENTS_HPP
