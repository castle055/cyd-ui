/*! \file  layout.cppm
 *! \brief 
 *!
 */

module;
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


export module cydui:layout;

import std;
import fabric.logging;
import fabric.profiling;

export import cydui.application;
export import cydui.components;
export import cydui.dimensions;

import cydui.window_events;

export import :window;

export namespace cyd::ui {
  template<components::ComponentConcept C>
  Layout* create(C &&root_component);

  template<components::ComponentConcept C>
  Layout* create(C &root_component);

  class Layout {
    Layout(
      const components::component_state_ref &_root_state,
      const components::component_base_t::sptr &_root
    )
      : root_state(_root_state),
        root(_root),
        focused(_root_state) {
      focused->focused = true;
    }

  public:
    ~Layout() {
      for (auto &item: listeners) {
        item->remove();
      }
    }

  public:
    components::component_base_t* find_by_coords(dimensions::screen_measure x, dimensions::screen_measure y);

    bool update_if_dirty(components::component_base_t* c);

    bool render_if_dirty(components::component_base_t* c);

    void redraw_component(components::component_base_t* target);

    static void recompute_dimensions(const components::component_base_t::sptr &start_from);

    static void clear_hovering_flag(const components::component_state_ref &state, const MotionEvent &ev);

    static bool set_hovering_flag(
      components::component_state_t* state,
      const MotionEvent &ev,
      bool clear_children = true
    );

  private:
    void render();

  public:
    template<components::ComponentConcept C>
    friend Layout* create(C &&root_component);

    template<components::ComponentConcept C>
    friend Layout* create(C &root_component);

  public:
    // drag_n_drop::dragging_context_t dragging_context {};

    void bind_window(const CWindow::sptr &_win);

    components::component_state_ref get_root_state() const {
      return root_state;
    }

  private:
    std::shared_ptr<CWindow> win = nullptr;

    components::component_state_ref root_state;
    components::component_base_t::sptr root;

    components::component_state_ref hovering = nullptr;
    components::component_state_ref focused  = nullptr;

    std::vector<fabric::async::raw_listener::sptr> listeners { };

    std::atomic_flag is_compositing {false};
    std::atomic_flag composite_is_outdated {false};
  };
}

//* IMPL

export namespace cyd::ui
{
  template <components::ComponentConcept C>
  Layout* create(C&& root_component) {
    auto root                      = std::shared_ptr<C>{new C{root_component}};
    auto root_state                = root->create_state_instance();
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }

  template <components::ComponentConcept C>
  Layout* create(C& root_component) {
    auto root                      = std::shared_ptr<C>{new C{root_component}};
    auto root_state                = root->create_state_instance();
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }
}

namespace cyd::ui {
#define COMPUTE(DIM)                                                                               \
  {                                                                                                \
    auto compute_res = cyd::ui::dimensions::compute_dimension(DIM);                                         \
    if (not compute_res) {                                                                         \
      return false;                                                                                \
    }                                                                                              \
  }

  static bool compute_dimensions(cyd::ui::components::component_base_t* rt) {
    using namespace cyd::ui::dimensions;
    auto &dim     = rt->get_dimensional_relations();
    auto &int_rel = rt->get_internal_relations();

    /// COMPUTE SOME VALUES
    COMPUTE(dim._x)
    COMPUTE(dim._y)

    COMPUTE(dim._margin_top)
    COMPUTE(dim._margin_right)
    COMPUTE(dim._margin_bottom)
    COMPUTE(dim._margin_left)

    COMPUTE(dim._padding_top)
    COMPUTE(dim._padding_right)
    COMPUTE(dim._padding_bottom)
    COMPUTE(dim._padding_left)

    /// COMPUTE CHILDREN ORIGIN POINT (cx, cy)
    if (rt->parent.has_value()) {
      auto &parent_dim     = rt->parent.value()->get_dimensional_relations();
      auto &parent_int_rel = rt->parent.value()->get_internal_relations();

      int_rel.cx = dimensions::get_value(parent_int_rel.cx) + dimensions::get_value(dim._x)
                   + dimensions::get_value(dim._margin_left)
                   + dimensions::get_value(dim._padding_left);
      int_rel.cy = dimensions::get_value(parent_int_rel.cy) + dimensions::get_value(dim._y)
                   + dimensions::get_value(dim._margin_top) + dimensions::get_value(dim._padding_top);
    } else {
      int_rel.cx = dimensions::get_value(dim._x) + dimensions::get_value(dim._margin_left)
                   + dimensions::get_value(dim._padding_left);
      int_rel.cy = dimensions::get_value(dim._y) + dimensions::get_value(dim._margin_top)
                   + dimensions::get_value(dim._padding_top);
    }
    COMPUTE(int_rel.cx)
    COMPUTE(int_rel.cy)

    /// COMPUTE SIZE
    if (dim._width.is_set()) {
      COMPUTE(dim._width)
      int_rel.cw = dimensions::get_value(dim._width) - dimensions::get_value(dim._padding_left) - dimensions::get_value(
                     dim._padding_right)
                   - dimensions::get_value(dim._margin_left) - dimensions::get_value(dim._margin_right);
      COMPUTE(int_rel.cw)
    }
    if (dim._height.is_set()) {
      COMPUTE(dim._height)
      int_rel.ch = dimensions::get_value(dim._height) - dimensions::get_value(dim._padding_top) - dimensions::get_value(
                     dim._padding_bottom)
                   - dimensions::get_value(dim._margin_top) - dimensions::get_value(dim._margin_bottom);
      COMPUTE(int_rel.ch)
    }

    /// COMPUTE DIMENSIONS FOR CHILDREN RECURSIVELY
    std::vector<std::shared_ptr<components::component_base_t>> pending;

    auto total_w = 0_px;
    auto total_h = 0_px;
    for (auto &child: rt->children) {
      //compute_dimensions(child);
      // if error (circular dependency), skip for now, and then recalculate
      if (compute_dimensions(child.get())) {
        auto &c_dim      = child->get_dimensional_relations();
        auto child_max_w = dimensions::get_value(c_dim._x) + dimensions::get_value(c_dim._width);
        auto child_max_h = dimensions::get_value(c_dim._y) + dimensions::get_value(c_dim._height);
        total_w          = std::max(total_w, child_max_w);
        total_h          = std::max(total_h, child_max_h);
      } else {
        pending.push_back(child);
      }
    }

    if (not dim._width.is_set()) {
      // If not given, or given has error (ie: circular dep)
      int_rel.cw = total_w;
      COMPUTE(int_rel.cw)
      dim._width = dimensions::get_value(int_rel.cw) + dimensions::get_value(dim._padding_left) + dimensions::get_value(
                     dim._padding_right)
                   + dimensions::get_value(dim._margin_left) + dimensions::get_value(dim._margin_right);
      COMPUTE(dim._width)
    }

    if (not dim._height.is_set()) {
      // If not given, or given has error (ie: circular dep)
      int_rel.ch = total_h;
      COMPUTE(int_rel.ch)
      dim._height = dimensions::get_value(int_rel.ch) + dimensions::get_value(dim._padding_top) + dimensions::get_value(
                      dim._padding_bottom)
                    + dimensions::get_value(dim._margin_top) + dimensions::get_value(dim._margin_bottom);
      COMPUTE(dim._height)
    }

    return std::all_of(pending.begin(), pending.end(), [](const auto &it) { return compute_dimensions(it.get()); });
  }

#undef COMPUTE

  void cyd::ui::Layout::recompute_dimensions(
    const components::component_base_t::sptr &start_from
  ) {
    if (!compute_dimensions(start_from.get()) && start_from->parent.has_value()) {
      components::component_base_t* c = start_from->parent.value();
      while (c && !compute_dimensions(c)) {
        if (!c->parent.has_value()) {
          LOG::print {ERROR}("Could not compute dimensions");
          // TODO - Catch dimensional error
        }
        c = c->parent.value();
      }
    }
  }

  void Layout::render() {
    {
      ZoneScopedN("Dimensions");
      recompute_dimensions(root);
    }
    if (is_compositing.test()) {
      composite_is_outdated.test_and_set();
      return;
    } {
      ZoneScopedN("Updating Fragments");
      root->update_fragment(nullptr);
    } {
      ZoneScopedN("Start Render");

      Application::run([](CWindow* w, components::component_base_t* root_) {
        ZoneScopedN("Start layout render");
        root_->start_render(w->native());
      }, win.get(), root.get());

    } {
      ZoneScopedN("Render");

      root->render(win->native());
    } {
      ZoneScopedN("Queuing Composition");
      //compositing_tree->fix_dimensions();
      is_compositing.test_and_set();
      Application::run_async([](Layout* self, std::atomic_flag* completion_flag, std::atomic_flag* is_outdated, CWindow* w, components::component_base_t* root_ptr) {
        ZoneScopedN("Compositing Layout");
        bool must_recompose = false;
        auto* root_node     = root_ptr->compose(w->native(), &must_recompose);

        if (must_recompose) {
          ZoneScopedN("Compositing Frame");
          w->compositor.compose(root_node);
        }

        completion_flag->clear();
        if (is_outdated->test()) {
          //-IMPORTANT: Need to make copy of pointers so they aren't passed
          // as references which will not survive
          Layout& s = *self;
          std::atomic_flag& flag = *is_outdated;
          //---------------------------------------------------------------
          w->run_async([](Layout* selff, std::atomic_flag* is_outdated) {
            selff->render();
            is_outdated->clear();
          }, &s, &flag);
        }
      }, this, &is_compositing, &composite_is_outdated, win.get(), root.get());
    }
  }

  void cyd::ui::Layout::redraw_component(components::component_base_t* target) {
    ZoneScopedN("Redraw Component");
    {
      ZoneScopedN("Update");
      target->redraw();
    }
    render();
  }

  bool cyd::ui::Layout::update_if_dirty(components::component_base_t* c) {
    if (c->state()->_dirty) {
      c->redraw();
      return true;
    } else {
      bool any = false;
      for (auto &item: c->children)
        any = update_if_dirty(item.get()) || any; // ! Order here matters
      // ? render_if_dirty() needs to be called before `any` is checked.
      return any;
    }
  }

  bool cyd::ui::Layout::render_if_dirty(components::component_base_t* c) {
    ZoneScopedN("Render If Dirty");
    {
      ZoneScopedN("Update");
      if (not update_if_dirty(c)) {
        return false;
      }
    }
    render();
    return true;
  }

  components::component_base_t* cyd::ui::Layout::find_by_coords(
    dimensions::screen_measure x,
    dimensions::screen_measure y
  ) {
    ZoneScopedN("Find by coords");
    return root->find_by_coords(x, y);
  }

//static event_handler_t* get_instance_ev_handler(component_state_t* component_state) {
//
//}

#define INSTANCE_EV_HANDLER(STATE_PTR) \
  if (STATE_PTR->component_instance.has_value()) \
    STATE_PTR->component_instance.value()


  void cyd::ui::Layout::bind_window(const cyd::ui::CWindow::sptr &_win) {
    this->win = _win; {
      /// Configure root component
      root_state->window = this->win;

      auto [w, h] = win->get_size();
      auto &dim   = root->get_dimensional_relations();
      dim._width  = cyd::ui::dimensions::screen_measure {double(w)}; //{};
      dim._height = cyd::ui::dimensions::screen_measure {double(h)}; //{};
      root->configure_event_handler();
      recompute_dimensions(root);
    }

    for (auto &item: listeners) {
      item->remove();
    }
    listeners.clear();

    auto make_listener = [&](auto &&fun) { return win->on_event(fun).raw(); };

    listeners = {
      make_listener([&](const RedrawEvent &ev) {
        ZoneScopedN("Redraw Event");
        auto _pev = this->win->profiling_ctx.scope_event("Redraw");
        if (ev.component) {
          components::component_state_t* target_state = ((components::component_state_t*)ev.component);
          if (target_state->component_instance.has_value()) {
            redraw_component(target_state->component_instance.value().get());
          }
        } else {
          redraw_component(root.get());
        }
      }),
      make_listener([&](const KeyEvent &ev) {
        ZoneScopedN("Key Event");
        auto _pev = this->win->profiling_ctx.scope_event("Key");
        if (ev.keysym.code == Keycode::SDLK_F12 && ev.pressed && not ev.holding) {
          LOG::print {INFO}("Pressed Debug Key");
          return;
        }
        if (focused && focused->component_instance) {
          if (focused->focused) {
            if (ev.pressed) {
              INSTANCE_EV_HANDLER(focused)->dispatch_key_press(ev);
            } else if (ev.released) {
              INSTANCE_EV_HANDLER(focused)->dispatch_key_release(ev);
            }
            render_if_dirty(focused->component_instance.value().get());
          } else {
            focused = nullptr;
          }
        }
      }),
      make_listener([&](const TextInputEvent &ev) {
        ZoneScopedN("Key Event");
        auto _pev = this->win->profiling_ctx.scope_event("Key");
        if (focused && focused->component_instance) {
          if (focused->focused) {
            INSTANCE_EV_HANDLER(focused)->dispatch_text_input(ev);
            render_if_dirty(focused->component_instance.value().get());
          } else {
            focused = nullptr;
          }
        }
      }),
      make_listener([&](const ButtonEvent &ev) {
        ZoneScopedN("Button Event");
        auto _pev = this->win->profiling_ctx.scope_event("Button");

        components::component_base_t* target           = root.get();
        components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }

        auto &dim     = target->get_dimensional_relations();
        auto &int_rel = target->get_internal_relations();
        auto rel_x    = ev.x - dimensions::get_value(int_rel.cx);
        auto rel_y    = ev.y - dimensions::get_value(int_rel.cy);

        if (focused != target->state()) {
          if (focused) {
            // if (focused->component_instance.has_value()) {
            //   focused->component_instance.value()
            //     ->event_handler()
            //     ->on_button_release((Button) it.button, 0, 0);
            // }
            focused->focused = false;
            focused->mark_dirty();
            focused = nullptr;
          }
          focused          = target->state();
          focused->focused = true;
          focused->mark_dirty();
        }

        if (ev.pressed) {
          target->dispatch_button_press((Button)ev.button, rel_x, rel_y);
        } else {
          target->dispatch_button_release((Button)ev.button, rel_x, rel_y);
        }
        render_if_dirty(root.get());
      }),
      make_listener([&](const ScrollEvent &ev) {
        ZoneScopedN("Scroll Event");
        auto _pev                                      = this->win->profiling_ctx.scope_event("Scroll");
        components::component_base_t* target           = root.get();
        components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
        if (specified_target) {
          target = specified_target;
        }

        target->dispatch_scroll(ev.dx, ev.dy);

        render_if_dirty(root.get());
      }),
      make_listener([&](const MotionEvent &ev) {
        auto _pev = this->win->profiling_ctx.scope_event("Motion");
        ZoneScopedN("MotionEvent");

        if (ev.x == dimensions::screen_measure {-1} && ev.y == dimensions::screen_measure {-1}) {
          clear_hovering_flag(root_state, ev);
        } else {
          components::component_base_t* target           = root.get();
          components::component_base_t* specified_target = find_by_coords(ev.x, ev.y);
          if (specified_target)
            target = specified_target;

          if (not set_hovering_flag(target->state().get(), ev, true)) {
            auto &int_rel = target->get_internal_relations();
            auto rel_x    = ev.x - dimensions::get_value(int_rel.cx);
            auto rel_y    = ev.y - dimensions::get_value(int_rel.cy);
            target->dispatch_mouse_motion(rel_x, rel_y);
          }
        }

        // Calling 'Drag' related event handlers
        // cyd::ui::components::Component* target = root;
        // cyd::ui::components::Component* specified_target =
        //  find_by_coords(root, it.x, it.y);
        // if (specified_target)
        //  target = specified_target;
        //
        // if (it.dragging) {
        //  if (dragging_context.dragging) {
        //    int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //    int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
        //    dragging_context.dragging_item.drag_move(dragging_context.dragging_item, rel_x,
        //    rel_y); target->on_drag_motion(rel_x, rel_y);
        //  } else {
        //    int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //    int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
        //    target->state.let(_(components::ComponentState * , {
        //      for (auto &item : it->draggable_sources) {
        //        if (item.x - 10 <= rel_x && rel_x <= item.x + 10
        //          && item.y - 10 <= rel_y && rel_y <= item.y + 10) {
        //          dragging_context.dragging_item = item.start_drag(rel_x, rel_y);
        //          break;
        //        }
        //      }
        //    }));
        //    target->on_drag_start(rel_x, rel_y);
        //    dragging_context.dragging = true;
        //  }
        //} else if (dragging_context.dragging) {
        //  int rel_x = it.x - (*target->state.unwrap())->dim.cx.val();
        //  int rel_y = it.y - (*target->state.unwrap())->dim.cy.val();
        //  dragging_context.dragging_item.drag_end(dragging_context.dragging_item, rel_x, rel_y);
        //  target->on_drag_finish(rel_x, rel_y);
        //  dragging_context.dragging = false;
        //  dragging_context.dragging_item = drag_n_drop::draggable_t {};
        //}

        render_if_dirty(root.get());
      }),
      make_listener([&](const ResizeEvent &ev) {
        ZoneScopedN("Resize Event");
        auto _pev = this->win->profiling_ctx.scope_event("Resize");

        auto &dim   = root->get_dimensional_relations();
        dim._width  = ev.w;
        dim._height = ev.h;

        recompute_dimensions(root);
      }),
    };
  }

  bool Layout::set_hovering_flag(components::component_state_t* state, const MotionEvent &ev, bool clear_children) {
    if (clear_children) {
      for (const auto &c_state: std::ranges::views::values(state->children_states)) {
        clear_hovering_flag(c_state, ev);
      }
    }

    if (not state->hovering) {
      state->hovering = true;

      if (state->component_instance.has_value()) {
        auto &int_rel = state->component_instance.value()->get_internal_relations();
        auto rel_x    = ev.x - dimensions::get_value(int_rel.cx);
        auto rel_y    = ev.y - dimensions::get_value(int_rel.cy);
        state->component_instance.value()->dispatch_mouse_enter(rel_x, rel_y);
      }

      state->mark_dirty();

      if (state->parent()) {
        if (state->parent()->hovering) {
          for (auto &[id, c_state]: state->parent()->children_states) {
            if (c_state.get() != state) {
              clear_hovering_flag(c_state, ev);
            }
          }
        } else {
          set_hovering_flag(state->parent(), ev, false);
        }
      }

      return true;
    }

    return false;
  }

  void Layout::clear_hovering_flag(const components::component_state_ref &state, const MotionEvent &ev) {
    if (state->hovering) {
      state->hovering = false;

      if (state->component_instance.has_value()) {
        auto &h_int_rel = state->component_instance.value()->get_internal_relations();
        auto exit_rel_x = ev.x - dimensions::get_value(h_int_rel.cx);
        auto exit_rel_y = ev.y - dimensions::get_value(h_int_rel.cy);
        state->component_instance.value()->dispatch_mouse_exit(exit_rel_x, exit_rel_y);
      }

      state->mark_dirty();

      for (const auto &c_state: std::ranges::views::values(state->children_states)) {
        clear_hovering_flag(c_state, ev);
      }
    }
  }

  template<typename Component>
  CWindow::builder_t CWindow::make(typename Component::props_t props) {
    auto layout = cyd::ui::create(Component {props});
    return CWindow::builder_t(layout);
  }

  void bind_layout(Layout* layout, const CWindow::sptr &window) {
    layout->bind_window(window);
  }
}
