/*! \file  core.cppm
 *! \brief 
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>

export module cydui.core;

import std;
import fabric.logging;
import fabric.async;
import fabric.profiling;

export import cydui.components;
export import cydui.dimensions;

export namespace cyd::ui {
  namespace window {
      class CWindow;
  }

  namespace layout {
      class Layout;
  }
}

export {
namespace cyd::ui::window {
  class CWindow: public fabric::async::async_bus_t {
  public:
    using sptr = std::shared_ptr<CWindow>;

    graphics::window_t* win_ref;
    layout::Layout* layout;

    prof::context_t profiling_ctx{};

    void terminate();

    bool is_open() const;

    std::pair<int, int> get_position() const;
    std::pair<int, int> get_size() const;
  };

  CWindow::sptr create(
    layout::Layout* layout,
    const char* title = "CydUI",
    const char* wclass = "cydui",
    int x = 0,
    int y = 0,
    int w = 640,
    int h = 480,
    bool override_redirect = false
  );
}

}


export namespace cyd::ui {
  namespace layout {
    template<components::ComponentConcept C>
    Layout* create(C &&root_component);

    template<components::ComponentConcept C>
    Layout* create(C &root_component);

    class Layout {
      std::shared_ptr<window::CWindow> win = nullptr;

      compositing::LayoutCompositor compositor{};

      components::component_state_ref    root_state;
      components::component_base_t::sptr root;

      components::component_state_ref hovering = nullptr;
      components::component_state_ref focused  = nullptr;

      std::vector<fabric::async::raw_listener::sptr> listeners{};

      components::component_base_t*
      find_by_coords(dimensions::screen_measure x, dimensions::screen_measure y);

      Layout(
        const components::component_state_ref&    _root_state,
        const components::component_base_t::sptr& _root
      )
          : root_state(_root_state),
            root(_root),
            focused(_root_state) {
        focused->focused = true;
      }

      bool render_if_dirty(components::component_base_t* c);

      void redraw_component(components::component_base_t* target);

      static void recompute_dimensions(const components::component_base_t::sptr& start_from);

      static void clear_hovering_flag(const components::component_state_ref& state);
      static void set_hovering_flag(const components::component_state_ref& state);

      template <components::ComponentConcept C>
      friend Layout* layout::create(C&& root_component);

      template <components::ComponentConcept C>
      friend Layout* layout::create(C& root_component);

    public:
      ~Layout() {
        for (auto& item: listeners) {
          item->remove();
        }
      }

      // drag_n_drop::dragging_context_t dragging_context {};

      void bind_window(const window::CWindow::sptr& _win);

      components::component_state_ref get_root_state() const {
        return root_state;
      }
    };
  }
}

//* IMPL

export namespace cyd::ui
{
  template <components::ComponentConcept C>
  layout::Layout* layout::create(C&& root_component) {
    auto root                      = std::shared_ptr<C>{new C{root_component}};
    auto root_state                = root->create_state_instance();
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }

  template <components::ComponentConcept C>
  layout::Layout* layout::create(C& root_component) {
    auto root                      = std::shared_ptr<C>{new C{root_component}};
    auto root_state                = root->create_state_instance();
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }
}


#define COMPUTE(DIM)                                                                               \
  {                                                                                                \
    auto compute_res = dimensions::compute_dimension(DIM);                                         \
    if (not compute_res) {                                                                         \
      return false;                                                                                \
    }                                                                                              \
  }

using namespace cyd::ui;
using namespace cyd::ui::components;

static bool compute_dimensions(component_base_t* rt) {
  using namespace cyd::ui::dimensions;
  auto& dim = rt->get_dimensional_relations();
  auto& int_rel = rt->get_internal_relations();

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
    auto& parent_dim     = rt->parent.value()->get_dimensional_relations();
    auto& parent_int_rel = rt->parent.value()->get_internal_relations();

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
    int_rel.cw = dimensions::get_value(dim._width) - dimensions::get_value(dim._padding_left) - dimensions::get_value(dim._padding_right)
      - dimensions::get_value(dim._margin_left) - dimensions::get_value(dim._margin_right);
    COMPUTE(int_rel.cw)
  }
  if (dim._height.is_set()) {
    COMPUTE(dim._height)
    int_rel.ch = dimensions::get_value(dim._height) - dimensions::get_value(dim._padding_top) - dimensions::get_value(dim._padding_bottom)
      - dimensions::get_value(dim._margin_top) - dimensions::get_value(dim._margin_bottom);
    COMPUTE(int_rel.ch)
  }

  /// COMPUTE DIMENSIONS FOR CHILDREN RECURSIVELY
  std::vector<std::shared_ptr<component_base_t>> pending;

  auto total_w = 0_px;
  auto total_h = 0_px;
  for (auto &child: rt->children) {
    //compute_dimensions(child);
    // if error (circular dependency), skip for now, and then recalculate
    if (compute_dimensions(child.get())) {
      auto& c_dim = child->get_dimensional_relations();
      auto child_max_w = dimensions::get_value(c_dim._x) + dimensions::get_value(c_dim._width);
      auto child_max_h = dimensions::get_value(c_dim._y) + dimensions::get_value(c_dim._height);
      total_w = std::max(total_w, child_max_w);
      total_h = std::max(total_h, child_max_h);
    } else {
      pending.push_back(child);
    }
  }

  if (not dim._width.is_set()) {
    // If not given, or given has error (ie: circular dep)
    int_rel.cw = total_w;
    COMPUTE(int_rel.cw)
    dim._width = dimensions::get_value(int_rel.cw) + dimensions::get_value(dim._padding_left) + dimensions::get_value(dim._padding_right)
      + dimensions::get_value(dim._margin_left) + dimensions::get_value(dim._margin_right);
    COMPUTE(dim._width)
  }

  if (not dim._height.is_set()) {
    // If not given, or given has error (ie: circular dep)
    int_rel.ch = total_h;
    COMPUTE(int_rel.ch)
    dim._height = dimensions::get_value(int_rel.ch) + dimensions::get_value(dim._padding_top) + dimensions::get_value(dim._padding_bottom)
      + dimensions::get_value(dim._margin_top) + dimensions::get_value(dim._margin_bottom);
    COMPUTE(dim._height)
  }

  return std::all_of(pending.begin(), pending.end(), [](const auto& it) { return compute_dimensions(it.get()); });
}

#undef COMPUTE

void cyd::ui::layout::Layout::recompute_dimensions(
  const component_base_t::sptr& start_from
) {
  if (!compute_dimensions(start_from.get()) && start_from->parent.has_value()) {
    component_base_t* c = start_from->parent.value();
    while (c && !compute_dimensions(c)) {
      if (!c->parent.has_value()) {
        LOG::print{ERROR}("Could not compute dimensions");
        // TODO - Catch dimensional error
      }
      c = c->parent.value();
    }
  }
}

void cyd::ui::layout::Layout::redraw_component(component_base_t* target) {
  LOG::print{DEBUG}("REDRAW");
  //auto t0 = std::chrono::system_clock::now();
  // Clear render area of component instances
  auto* compositing_tree = new compositing::compositing_tree_t;

  // TODO - For now the entire screen is redraw everytime, in the future it
  // would be interesting to implement a diff algorithm that could redraw
  // subsections of the screen.
  // target->clear_children();
  // Recreate those instances with redraw(), this set all size hints relationships
  target->redraw();

  recompute_dimensions(root);

  root->get_fragment(compositing_tree->root);
  //compositing_tree->fix_dimensions();

  compositor.compose(compositing_tree);
  //auto t1 = std::chrono::system_clock::now();
  //printf("redraw time: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
}

bool cyd::ui::layout::Layout::render_if_dirty(component_base_t* c) {
  if (c->state()->_dirty) {
    redraw_component(c);
    return true;
  } else {
    bool any = false;
    for (auto &item: c->children)
      any = render_if_dirty(item.get()) || any; // ! Order here matters
    // ? render_if_dirty() needs to be called before `any` is checked.
    return any;
  }
}

component_base_t* cyd::ui::layout::Layout::find_by_coords(dimensions::screen_measure x, dimensions::screen_measure y) {
  return root->find_by_coords(x, y);
}

//static event_handler_t* get_instance_ev_handler(component_state_t* component_state) {
//
//}

#define INSTANCE_EV_HANDLER(STATE_PTR) \
  if (STATE_PTR->component_instance.has_value()) \
    STATE_PTR->component_instance.value()


void cyd::ui::layout::Layout::bind_window(const cyd::ui::window::CWindow::sptr& _win) {
  this->win = _win;
  compositor.set_render_target(this->win->win_ref, &this->win->profiling_ctx);
  {
    /// Configure root component
    root_state->window = this->win;

    auto& dim           = root->get_dimensional_relations();
    dim._width         = {(int)get_frame(this->win->win_ref)->width()};
    dim._height        = {(int)get_frame(this->win->win_ref)->height()};
    root->configure_event_handler();
  }

  for (auto& item: listeners) {
    item->remove();
  }
  listeners.clear();

  auto make_listener = [&](auto&& fun) { return win->on_event(fun).raw(); };

  listeners          = {
    make_listener([&](const RedrawEvent& ev) {
      auto _pev = this->win->profiling_ctx.scope_event("Redraw");
      if (ev.component) {
        component_state_t* target_state = ((component_state_t*)ev.component);
        if (target_state->component_instance.has_value()) {
          redraw_component(target_state->component_instance.value().get());
        }
      } else {
        redraw_component(root.get());
      }
    }),
    make_listener([&](const KeyEvent& ev) {
      auto _pev = this->win->profiling_ctx.scope_event("Key");
      if (ev.key == Key::F12 && ev.pressed && not ev.holding) {
        LOG::print {INFO} ("Pressed Debug Key");
        return;
      }
      if (focused && focused->component_instance) {
        if (focused->focused) {
          if (ev.pressed) {
            INSTANCE_EV_HANDLER(focused)->dispatch_key_press(ev);
          } else if (ev.released) {
            INSTANCE_EV_HANDLER(focused)->dispatch_key_release(ev);
          }
        } else {
          focused = nullptr;
        }
      }
    }),
    make_listener([&](const ButtonEvent& ev) {
      auto              _pev             = this->win->profiling_ctx.scope_event("Button");

      component_base_t* target           = root.get();
      component_base_t* specified_target = find_by_coords(ev.x, ev.y);
      if (specified_target) {
        target = specified_target;
      }

      auto& dim     = target->get_dimensional_relations();
      auto& int_rel = target->get_internal_relations();
      auto  rel_x   = ev.x - dimensions::get_value(int_rel.cx);
      auto  rel_y   = ev.y - dimensions::get_value(int_rel.cy);

      if (focused != target->state()) {
        if (focused) {
          // if (focused->component_instance.has_value()) {
          //   focused->component_instance.value()
          //     ->event_handler()
          //     ->on_button_release((Button) it.button, 0, 0);
          // }
          focused->focused = false;
          focused          = nullptr;
        }
        focused          = target->state();
        focused->focused = true;
      }

      if (ev.pressed) {
        target->dispatch_button_press((Button)ev.button, rel_x, rel_y);
      } else {
        target->dispatch_button_release((Button)ev.button, rel_x, rel_y);
      }
      render_if_dirty(root.get());
    }),
    make_listener([&](const ScrollEvent& ev) {
      auto              _pev             = this->win->profiling_ctx.scope_event("Scroll");
      component_base_t* target           = root.get();
      component_base_t* specified_target = find_by_coords(ev.x, ev.y);
      if (specified_target) {
        target = specified_target;
      }

      target->dispatch_scroll(ev.dx, ev.dy);

      render_if_dirty(root.get());
    }),
    make_listener([&](const MotionEvent& ev) {
      auto _pev = this->win->profiling_ctx.scope_event("Motion");

      if (ev.x == dimensions::screen_measure {-1} && ev.y == dimensions::screen_measure {-1}) {
        if (hovering && hovering->component_instance.has_value()) {
          int exit_rel_x     = 0;
          int exit_rel_y     = 0;
          hovering->hovering = false;
          hovering->component_instance.value()->dispatch_mouse_exit(0, 0);
          hovering = nullptr;
        }
      } else {
        component_base_t* target           = root.get();
        component_base_t* specified_target = find_by_coords(ev.x, ev.y);
        if (specified_target)
          target = specified_target;

        auto& dim     = target->get_dimensional_relations();
        auto& int_rel = target->get_internal_relations();
        auto  rel_x   = ev.x - dimensions::get_value(int_rel.cx);
        auto  rel_y   = ev.y - dimensions::get_value(int_rel.cy);

        if (hovering != target->state()) {
          if (hovering && hovering->component_instance.has_value()) {
            auto  h_dim        = hovering->component_instance.value()->get_dimensional_relations();
            auto& h_int_rel    = hovering->component_instance.value()->get_internal_relations();
            auto  exit_rel_x   = ev.x - dimensions::get_value(h_int_rel.cx);
            auto  exit_rel_y   = ev.y - dimensions::get_value(h_int_rel.cy);
            hovering->hovering = false;
            hovering->component_instance.value()->dispatch_mouse_exit(exit_rel_x, exit_rel_y);
            hovering = nullptr;
          }
          hovering           = target->state();
          hovering->hovering = true;

          target->dispatch_mouse_enter(rel_x, rel_y);
        } else {
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
    make_listener([&](const ResizeEvent& ev) {
      auto _pev = this->win->profiling_ctx.scope_event("Resize");
      LOG::print{DEBUG}("RESIZE: w={}, h={}", ev.w.to_string(), ev.h.to_string());

      auto& dim    = root->get_dimensional_relations();
      dim._width  = ev.w;
      dim._height = ev.h;

      redraw_component(root.get());
    }),
  };
}

void layout::Layout::set_hovering_flag(const component_state_ref &state) {
  if (state->parent() && state->parent()->hovering) {
    state->hovering = true;
    state->mark_dirty();
  }
  if (!state->hovering) {
    state->hovering = true;
    state->mark_dirty();

    for (const auto &c_state: std::ranges::views::values(state->children_states)) {
      clear_hovering_flag(c_state);
    }
  }
}

void layout::Layout::clear_hovering_flag(const component_state_ref &state) {
  if (state->hovering) {
    state->hovering = false;
    state->mark_dirty();

    for (const auto &c_state: std::ranges::views::values(state->children_states)) {
      clear_hovering_flag(c_state);
    }
  }
}


// WINDOW


using namespace cyd::ui::window;

void cyd::ui::window::CWindow::terminate() {
  delete layout;
  graphics::terminate(win_ref);
  win_ref = nullptr;
}

bool CWindow::is_open() const {
  return win_ref != nullptr;
}


std::pair<int, int> cyd::ui::window::CWindow::get_position() const {
  return graphics::get_position(win_ref);
}
std::pair<int, int> cyd::ui::window::CWindow::get_size() const {
  return graphics::get_size(win_ref);
}

CWindow::sptr cyd::ui::window::create(
  layout::Layout* layout,
  const char* title,
  const char* wclass,
  int x,
  int y,
  int w,
  int h,
  bool override_redirect
) {
  auto win = std::shared_ptr<CWindow>{new CWindow()};
  auto* win_ref = graphics::create_window(win.get(), &win->profiling_ctx, title, wclass, x, y, w, h, override_redirect);
  win->layout = layout;
  win->win_ref = win_ref;

  layout->bind_window(win);

  // Once all threads have started and everything is set up for this window
  // force a complete redraw
  //events::emit<RedrawEvent>({.win = (unsigned int) win_ref->xwin});
  //events::emit<RedrawEvent>({ });
  //events::emit<RedrawEvent>({.win = get_id(win_ref)});

  return win;
}

