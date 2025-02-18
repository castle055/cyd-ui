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
export import cydui.dimensions;
export import cydui.styling;

export import cydui.components;
export import cydui.components.renderer;
export import cydui.components.updater;
export import cydui.components.stylist;

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

      component_renderer->compositing_signal.connect([this](compositing::compositing_node_t *node) {
        this->win->compositor.compose(node);
      });
      component_updater->queue_render_signal.connect({
        component_renderer.get(), &components::component_renderer_t::queue_render
      });
      component_updater->apply_style_signal.connect({
        component_stylist.get(), &components::component_stylist_t::apply_style
      });
      component_updater->compiler_style_rules_signal.connect({
        component_stylist.get(), &components::component_stylist_t::compile_style_rule_list
      });
    }

  public:
    ~Layout() {
      for (auto &item: listeners) {
        item->remove();
      }
    }

  public:
    components::component_base_t* find_by_coords(dimensions::screen_measure x, dimensions::screen_measure y);

    bool update_all_dirty(const components::component_base_t::sptr& c);

    bool render_if_dirty(const components::component_base_t::sptr& c);

    void update_component(const components::component_base_t::sptr& target);

    static void clear_hovering_flag(const components::component_state_ref &state, const MotionEvent &ev);

    static bool set_hovering_flag(
      components::component_state_t* state,
      const MotionEvent &ev,
      bool clear_children = true
    );

    void attach_stylesheet(const StyleSheet::sptr& style_sheet);
  private:
    void update_dimensions();

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
    std::vector<fabric::async::raw_listener::sptr> make_event_listeners();

  private:
    CWindow::sptr win = nullptr;

    components::component_state_ref root_state;
    components::component_base_t::sptr root;

    components::component_state_ref hovering = nullptr;
    components::component_state_ref focused  = nullptr;

    std::vector<fabric::async::raw_listener::sptr> listeners { };

    StyleArchive::sptr style_archive {StyleArchive::make()};

    components::component_renderer_t::sptr component_renderer {components::component_renderer_t::make()};
    components::component_updater_t::sptr component_updater {components::component_updater_t::make()};
    components::component_stylist_t::sptr component_stylist {components::component_stylist_t::make()};
  };

//* IMPL

  template <components::ComponentConcept C>
  Layout* create(C&& root_component) {
    auto root                      = std::make_shared<C>(std::forward<C>(root_component));
    auto root_state                = cyd::ui::components::component_actor_t::create_state_instance(root.get());
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }

  template <components::ComponentConcept C>
  Layout* create(C& root_component) {
    auto root                      = std::make_shared<C>(root_component);
    auto root_state                = cyd::ui::components::component_actor_t::create_state_instance(root.get());
    root_state->component_instance = root;
    auto* lay                      = new Layout(root_state, root);
    return lay;
  }

#define COMPUTE(DIM)                                                                               \
  {                                                                                                \
    auto compute_res = cyd::ui::dimensions::compute_dimension(DIM);                                         \
    if (not compute_res) {                                                                         \
      return false;                                                                                \
    }                                                                                              \
  }

}

namespace cyd::ui {
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

  void Layout::update_dimensions() {
    ZoneScopedN("Dimensions");

    if (!compute_dimensions(root.get()) && root->parent.has_value()) {
      components::component_base_t* c = root->parent.value();
      while (c && !compute_dimensions(c)) {
        if (!c->parent.has_value()) {
          LOG::print {ERROR}("Could not compute dimensions");
          // TODO - Catch dimensional error
        }
        c = c->parent.value();
      }
    }
  }

  void Layout::update_component(const components::component_base_t::sptr& target) {
    ZoneScopedN("Update Component");
    component_updater->update(target, *style_archive);
  }

  bool Layout::update_all_dirty(const components::component_base_t::sptr& c) {
    if (c->state()->_dirty) {
      component_updater->update(c, *style_archive);
      return true;
    } else {
      bool any = false;
      for (auto &item: c->children)
        any = update_all_dirty(item) || any; // ! Order here matters
      // ? update_if_dirty() needs to be called before `any` is checked.
      return any;
    }
  }

  bool Layout::render_if_dirty(const components::component_base_t::sptr& c) {
    ZoneScopedN("Render If Dirty");
    {
      ZoneScopedN("Update");
      if (not update_all_dirty(c)) {
        return false;
      }
    }
    update_dimensions();
    component_renderer->render(*win->native(), root);
    return true;
  }

  components::component_base_t* Layout::find_by_coords(
    dimensions::screen_measure x,
    dimensions::screen_measure y
  ) {
    ZoneScopedN("Find by coords");
    return root->find_by_coords(x, y);
  }

  void Layout::attach_stylesheet(const StyleSheet::sptr &style_sheet) {
    style_archive->add_stylesheet(style_sheet);
  }


//static event_handler_t* get_instance_ev_handler(component_state_t* component_state) {
//
//}

  void Layout::bind_window(const cyd::ui::CWindow::sptr &_win) {
    this->win = _win; {
      /// Configure root component
      root_state->window = this->win;

      auto [w, h] = win->get_size();
      auto &dim   = root->get_dimensional_relations();
      dim._width  = cyd::ui::dimensions::screen_measure {double(w)}; //{};
      dim._height = cyd::ui::dimensions::screen_measure {double(h)}; //{};
      components::component_actor_t::mount_component(root.get());
      component_updater->update(root, *style_archive);
      update_dimensions();
      component_renderer->render(*win->native(), root);
    }

    listeners = make_event_listeners();
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
        state->component_instance.value()->get_event_dispatcher()->dispatch_mouse_enter(rel_x, rel_y);
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
        state->component_instance.value()->get_event_dispatcher()->dispatch_mouse_exit(exit_rel_x, exit_rel_y);
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
}

namespace cyd::ui {
  void bind_layout(Layout* layout, const CWindow::sptr &window) {
    layout->bind_window(window);
  }

  void CWindow::builder_t::configure_layout_style() {
    for (const auto& path: this->stylesheets_) {
      this->layout_->attach_stylesheet(StyleSheet::parse(path));
    }
    for (const auto& str: this->styles_) {
      this->layout_->attach_stylesheet(StyleSheet::parse(str));
    }
  }
}
