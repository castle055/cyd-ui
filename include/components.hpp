//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_COMPONENTS_HPP
#define CYD_UI_COMPONENTS_HPP

#include "children_state_collection.hpp"
#include "colors.hpp"
#include "dimensions.hpp"
#include "events.hpp"
#include "properties.hpp"
#include "window_types.hpp"
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

  template<typename c> requires ComponentConcept<c>
  struct c_init_t {
    typename c::Props props;

    std::optional<dimensions::dimensional_relation_t> x;
    std::optional<dimensions::dimensional_relation_t> y;
    std::optional<dimensions::dimensional_relation_t> w;
    std::optional<dimensions::dimensional_relation_t> h;

    std::vector<Component*> inner = {};

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
    inline c* create(c_init_t<c> init) {
        auto* _c = new c(
                (typename c::State*) (this->state->children.contains(ID)
                        ? (this->state->children[ID])
                        : (this->state->children.add(ID, new typename c::State()))),
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
    }

#define COMP(COMPONENT) create<COMPONENT, __COUNTER__>

    template<typename c, int ID, typename T>
    requires ComponentConcept<c>
    inline Component* create_for(
            T iter, std::function<c_init_t<c>(typename T::value_type)> block
    ) {
        int i = 0;
        auto temp_c = new Component();
        for (auto a = iter.begin(); a != iter.end(); ++a, ++i) {
            const c_init_t<c> init = block(*a);
            temp_c->children.push_back(
                    new c((typename c::State*) (this->state->children.contains(ID, i)
                                    ? (this->state->children.get_list(ID, i))
                                    : (this->state->children.add_list(
                                            ID, i, new typename c::State()))),
                            init.props,
                            [this, init](cydui::components::Component* __raw_local_) {
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
    }

#define FOR_EACH(COMPONENT) create_for<COMPONENT, __COUNTER__>
#define NULLCOMP            Component::new_group()


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

    void add(std::vector<Component*> children);

    std::vector<Component*> children;

    void redraw();

    void render(const cydui::window::CWindow* win);

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

    virtual void on_render(const cydui::window::CWindow* win);

    virtual void on_redraw();

    virtual void on_mouse_enter(int x, int y);

    virtual void on_mouse_exit(int x, int y);

    virtual void on_mouse_click(int x, int y, int button);

    virtual void on_scroll(int d);

    virtual void on_key_press();

    virtual void on_key_release();
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

#define RENDER(WIN) void on_render(const cydui::window::CWindow*(WIN)) override


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

#define C_NEW_ALL(ID, NAME, _PROPS, IN, INIT)                                  \
  new NAME(state->children.contains(ID)                                        \
          ? ((NAME##State*)state->children[ID])                                \
          : ((NAME##State*)state->children.add(ID, new NAME##State())),        \
      NAME::Props _PROPS,                                                      \
      [this](cydui::components::Component* __raw_local_##NAME) {               \
        this;                                                                  \
        state;                                                                 \
        auto* this##NAME = (NAME*)__raw_local_##NAME;                          \
                                                                               \
        std::vector<cydui::components::Component*> v IN;                       \
        this##NAME->add(v);                                                    \
                                                                               \
        std::function<void(NAME * this##NAME)> init = [this, state](           \
                                                          NAME* this##NAME) {  \
          this;                                                                \
          state;                                                               \
          INIT                                                                 \
        };                                                                     \
        init(this##NAME);                                                      \
      })

#define C_NEW_INNER(ID, NAME, PROPS, IN)                                       \
  C_NEW_ALL(ID, NAME, PROPS, IN, { state; })

#define C_NEW_PROPS(ID, NAME, PROPS) C_NEW_INNER(ID, NAME, PROPS, ({}))

#define C_NEW(ID, NAME) C_NEW_PROPS(ID, NAME, ({}))

#define C_GET_NEW_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define N(...)                                                                 \
  C_GET_NEW_MACRO(__VA_ARGS__, C_NEW_ALL, C_NEW_INNER, C_NEW_PROPS, C_NEW)     \
  (__COUNTER__, __VA_ARGS__)

#endif//CYD_UI_COMPONENTS_HPP
