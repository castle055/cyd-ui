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
      static std::function<void(Component*)> get_init_function(c_init_t<c> init, const component_builder_t &spec) {
        return [init, spec](cydui::components::Component* __raw_local_) {
          auto* local = (c*) __raw_local_;
          
          __raw_local_->state.let(_(ComponentState *, {
            if (spec.x.has_value())
              it->dim.x = spec.x.value();
            else if (init.x.has_value())
              it->dim.x = init.x.value();
            
            if (spec.y.has_value())
              it->dim.y = spec.y.value();
            else if (init.y.has_value())
              it->dim.y = init.y.value();
            
            if (spec.w.has_value()) {
              it->dim.w = spec.w.value();
              it->dim.given_w = true;
            } else if (init.w.has_value()) {
              it->dim.w = init.w.value();
              it->dim.given_w = true;
            }
            if (spec.h.has_value()) {
              it->dim.h = spec.h.value();
              it->dim.given_h = true;
            } else if (init.h.has_value()) {
              it->dim.h = init.h.value();
              it->dim.given_h = true;
            }
          }));
          
          local->add(init.inner);
          init.init(local);
        };
      }
    
    protected:

#define SET_REFERENCE if (init.ref) *(init.ref) =
#define SET_WINDOW(STATE) (STATE)->win = it->win
#define INSTANTIATE_COMP(STATE) new c(STATE, init.props, get_init_function(init))
      
      template<typename c, int ID>
      requires ComponentConcept<c>
      inline component_builder_t create(c_init_t<c> init) const {
        return {
          .x = init.x,
          .y = init.y,
          .w = init.w,
          .h = init.h,
          .build = *(state.let(_(ComponentState *, {
            auto* st = (typename c::State*) (it->children.contains(ID)
              ? (it->children[ID])
              : (it->children.add(ID, new typename c::State())));
            st->win = it->win;
            return st;
          })).let(_(typename c::State*, {
            SET_REFERENCE it;
            return [=](component_builder_t spec) {
              auto* _c = new c(it, init.props, get_init_function(init, spec));
              return _c;
            };
          })).unwrap())
        };
      }
      
      template<typename c, int ID, typename T>
      requires ComponentConcept<c>
      inline component_builder_t create_for(
        const T &iter, std::function<c_init_t<c>(typename T::value_type)> block
      ) const {
        std::vector<typename c::State*> states = {};
        state.let(_(ComponentState *, {
          int k = 0;
          for (auto a = iter.begin(); a != iter.end(); ++a, ++k) {
            auto* st =
              (typename c::State*) (it->children.contains(ID, k)
                ? (it->children.get_list(ID, k))
                : (it->children.add_list(ID, k, new typename c::State())));
            st->win = it->win;
            states.push_back(st);
          }
        }));
        
        return {
          .x = std::nullopt,
          .y = std::nullopt,
          .w = std::nullopt,
          .h = std::nullopt,
          .build = [iter, block, states](component_builder_t spec) {
            int i = 0;
            auto temp_c = Component::new_group();
            for (auto a = iter.begin(); a != iter.end(); ++a, ++i) {
              const c_init_t<c> init = block(*a);
              SET_REFERENCE nullptr;
              temp_c->children.push_back(
                new c(states[i], init.props, get_init_function(init, spec))
              );
            }
            return temp_c;
          },
        };
      }
      
      inline component_builder_t create_group(std::vector<component_builder_t> _children) const {
        return {
          .build = [_children](component_builder_t) {
            auto* group = Component::new_group();
            group->add(_children);
            return group;
          }
        };
      }

#undef SET_REFERENCE
#undef SET_WINDOW
#undef INSTANTIATE_COMP


#define NULLCOMP            {.x = std::nullopt, .y = std::nullopt, .w = std::nullopt, .h = std::nullopt, .build = [](component_builder_t){ return Component::new_group(); } }

#define COMP(COMPONENT) create<COMPONENT, __COUNTER__>

#define GROUP create_group

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
      
      mutable nullable<Component*> parent;
      mutable nullable<ComponentState*> state;
      cydui::dimensions::component_dimensions_t* dim;
      
      void add(const std::vector<component_builder_t> &ichildren, bool prepend = false) const;
      
      mutable std::deque<Component*> children;
      
      void redraw();
      
      void render(cydui::graphics::render_target_t* target) const;
      
      nullable<Component*> get_parent() const;

#define COMP_EVENT_HANDLER(EV, ARGS) virtual void on_##EV ARGS const
#define COMP_EVENT_HANDLER_OVERRIDE(EV, ARGS) virtual void on_##EV ARGS const override
      
      typedef typename KeyEvent::DataType KeyData;
      
      COMP_EVENT_HANDLER(redraw, ());
      
      COMP_EVENT_HANDLER(render, (cydui::graphics::render_target_t * target));
      
      COMP_EVENT_HANDLER(mouse_enter, (int x, int y));
      
      COMP_EVENT_HANDLER(mouse_exit, (int x, int y));
      
      COMP_EVENT_HANDLER(mouse_motion, (int x, int y));
      
      COMP_EVENT_HANDLER(mouse_click, (int x, int y, int button));
      
      COMP_EVENT_HANDLER(scroll, (int dx, int dy));
      
      COMP_EVENT_HANDLER(key_press, (KeyData key));
      
      COMP_EVENT_HANDLER(key_release, (KeyData key));

#define REDRAW                  COMP_EVENT_HANDLER_OVERRIDE(redraw, ())
#define RENDER(TARGET)          COMP_EVENT_HANDLER_OVERRIDE(render, (cydui::graphics::render_target_t * (TARGET)))
#define ON_KEY_RELEASE(KEY)     COMP_EVENT_HANDLER_OVERRIDE(key_release, (KeyData (KEY)))
#define ON_KEY_PRESS(KEY)       COMP_EVENT_HANDLER_OVERRIDE(key_press, (KeyData (KEY)))
#define ON_SCROLL(DX, DY)       COMP_EVENT_HANDLER_OVERRIDE(scroll, (int (DX), int (DY)))
#define ON_CLICK(X, Y, BUTTON)  COMP_EVENT_HANDLER_OVERRIDE(mouse_click,  (int (X), int (Y), int (BUTTON)))
#define ON_MOUSE_MOTION(X, Y)   COMP_EVENT_HANDLER_OVERRIDE(mouse_motion, (int (X), int (Y)))
#define ON_MOUSE_EXIT(X, Y)     COMP_EVENT_HANDLER_OVERRIDE(mouse_exit,   (int (X), int (Y)))
#define ON_MOUSE_ENTER(X, Y)    COMP_EVENT_HANDLER_OVERRIDE(mouse_enter,  (int (X), int (Y)))
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

#define PROPS(...)                                                           \
  struct Props __VA_ARGS__;                                                          \
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

#define REFRESH \
if (state->win) { \
  events::emit<RedrawEvent>({ \
    .win = (unsigned int)(*state->win.unwrap())->xwin, \
  });           \
}               \


#endif//CYD_UI_COMPONENTS_HPP
