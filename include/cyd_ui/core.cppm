/*! \file  core.cppm
 *! \brief 
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>
#include <tracy/Tracy.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


export module cydui.core;

import std;
import fabric.logging;
import fabric.async;
import fabric.profiling;

export import cydui.components;
export import cydui.dimensions;

import cydui.window_events;

export namespace cyd::ui {
  namespace window {
      class CWindow;
  }

  namespace layout {
      class Layout;
  }
}

void bind_layout(cyd::ui::layout::Layout* layout, const std::shared_ptr<cyd::ui::window::CWindow>& window);

export {
namespace cyd::ui::window {
  class CWindow: public fabric::async::async_bus_t {
  public:
    using sptr = std::shared_ptr<CWindow>;

    CWindow(int width, int height) {
      win_ref = new graphics::window_t(this, nullptr, 0, width, height);
    }

    ~CWindow() {
      this->stop();
    }

    static sptr make(int width, int height) {
      auto ptr = std::make_shared<CWindow>(width, height);
      ptr->add_system([ptr] {
        window_events::poll_events(*ptr);
      });

      ptr->add_init([] {
        SDL_SetMainReady();
        if (0 != SDL_Init(SDL_INIT_VIDEO)) {
          SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        }
      });

      ptr->add_init([ptr] {
        ptr->win_ref->window = SDL_CreateWindow(
          "Compositing",
          SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED,
          ptr->win_ref->staging_target->width(),
          ptr->win_ref->staging_target->height(),
          SDL_WINDOW_RESIZABLE
        );
        SDL_GetWindowID(ptr->win_ref->window);
        ptr->win_ref->renderer = SDL_CreateRenderer(ptr->win_ref->window, -1, SDL_RENDERER_ACCELERATED);

        // SDL_SetWindowTitle(rtarget->window, rtarget->title);
        // SDL_SetWindowPosition(rtarget->window, x, y);
        bind_layout(ptr->layout, ptr);
      });

      ptr->start();
      return ptr;
    }

    graphics::window_t* native() {
      return win_ref;
    }

    graphics::window_t* win_ref;
    layout::Layout* layout;

    prof::context_t profiling_ctx{};

    void terminate();

    bool is_open() const;

    std::pair<int, int> get_position() const;
    std::pair<int, int> get_size() const;

    compositing::LayoutCompositor compositor{};
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

      bool update_if_dirty(components::component_base_t* c);
      bool render_if_dirty(components::component_base_t* c);

      void redraw_component(components::component_base_t* target);

      static void recompute_dimensions(const components::component_base_t::sptr& start_from);

      static void clear_hovering_flag(const components::component_state_ref& state, const MotionEvent& ev);
      static bool set_hovering_flag(components::component_state_t* state, const MotionEvent& ev, bool clear_children = true);

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
  ZoneScopedN("Redraw Component");
  // LOG::print{DEBUG}("REDRAW");
  //auto t0 = std::chrono::system_clock::now();

  // TODO - For now the entire screen is redraw everytime, in the future it
  // would be interesting to implement a diff algorithm that could redraw
  // subsections of the screen.
  // target->clear_children();
  // Recreate those instances with redraw(), this set all size hints relationships
  compositing::compositing_node_t* root_node;
  bool must_recompose = false;
  {
    ZoneScopedN("Update");
    target->redraw();
  } {
    ZoneScopedN("Dimensions");
    recompute_dimensions(root);
  } {
    ZoneScopedN("Render");
    root->render();
  } {
    ZoneScopedN("Compositing Tree");
    //compositing_tree->fix_dimensions();
    root_node = root->compose(win->native(), &must_recompose);
  }
  if (must_recompose) {
    ZoneScopedN("Compositing Frame");
    win->compositor.compose(root_node);
  }

  //auto t1 = std::chrono::system_clock::now();
  //printf("redraw time: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
}

bool cyd::ui::layout::Layout::update_if_dirty(component_base_t* c) {
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

bool cyd::ui::layout::Layout::render_if_dirty(component_base_t* c) {
  ZoneScopedN("Render If Dirty");
  compositing::compositing_node_t* root_node;
  bool must_recompose = false;
  {
    ZoneScopedN("Update");
    if (not update_if_dirty(c)) {
      return false;
    }
  } {
    ZoneScopedN("Dimensions");
    recompute_dimensions(root);
  } {
    ZoneScopedN("Render");
    root->render(win->native());
  } {
    ZoneScopedN("Compositing Tree");
    //compositing_tree->fix_dimensions();
    root_node = root->compose(win->native(), &must_recompose);
  }
  if (must_recompose) {
    ZoneScopedN("Compositing Frame");
    win->compositor.compose(root_node);
  }
  return true;
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
  win->compositor.set_render_target(this->win->win_ref, &this->win->profiling_ctx);
  {
    /// Configure root component
    root_state->window = this->win;

    auto& dim           = root->get_dimensional_relations();
    dim._width         = 0_px; //{};
    dim._height        = 0_px; //{};
    root->configure_event_handler();
  }

  for (auto& item: listeners) {
    item->remove();
  }
  listeners.clear();

  auto make_listener = [&](auto&& fun) { return win->on_event(fun).raw(); };

  listeners          = {
    make_listener([&](const RedrawEvent& ev) {
      ZoneScopedN("Redraw Event");
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
      ZoneScopedN("Key Event");
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
      ZoneScopedN("Button Event");
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
      ZoneScopedN("Scroll Event");
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
      ZoneScopedN("MotionEvent");

      if (ev.x == dimensions::screen_measure {-1} && ev.y == dimensions::screen_measure {-1}) {
        clear_hovering_flag(root_state, ev);
      } else {
        component_base_t* target           = root.get();
        component_base_t* specified_target = find_by_coords(ev.x, ev.y);
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
    make_listener([&](const ResizeEvent& ev) {
      ZoneScopedN("Resize Event");
      auto _pev = this->win->profiling_ctx.scope_event("Resize");
      // LOG::print{DEBUG}("RESIZE: w={}, h={}", ev.w.to_string(), ev.h.to_string());

      auto& dim    = root->get_dimensional_relations();
      dim._width  = ev.w;
      dim._height = ev.h;

      recompute_dimensions(root);
      // redraw_component(root.get());
      // win->emit(RedrawEvent { });
    }),
  };

  // win->emit(RedrawEvent {});
  // redraw_component(root.get());
  // win->emit(ResizeEvent{
    // .w = win->get_size().first, //(int)get_frame(this->win->win_ref)->width(),
    // .h = (int)get_frame(this->win->win_ref)->height()
  // });
}

bool layout::Layout::set_hovering_flag(component_state_t* state, const MotionEvent& ev, bool clear_children) {
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

void layout::Layout::clear_hovering_flag(const component_state_ref &state, const MotionEvent& ev) {
  if (state->hovering) {
    state->hovering    = false;

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
  int x, y;
  SDL_GetWindowPosition(win_ref->window, &x, &y);
  return {x, y};
  // return graphics::get_position(win_ref);
}
std::pair<int, int> cyd::ui::window::CWindow::get_size() const {
  int w, h;
  SDL_GetWindowSize(win_ref->window, &w, &h);
  return {w, h};
  // return graphics::get_size(win_ref);
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
  // window_events::start_thread_if_needed();
  auto win = CWindow::make(w, h);
  // auto* win_ref = graphics::create_window(win.get(), &win->profiling_ctx, title, wclass, x, y, w, h, override_redirect);
  win->layout = layout;
  // win->win_ref = {};//win_ref;

  return win;
}

void bind_layout(cyd::ui::layout::Layout* layout, const cyd::ui::window::CWindow::sptr& window) {
  layout->bind_window(window);
}
