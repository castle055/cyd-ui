// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <tracy/Tracy.hpp>
#include "cyd_fabric_modules/headers/macros/async_events.h"

export module cydui.components.renderer;

import std;
import fabric.logging;
import fabric.wiring.signals;

import cydui.application;
import cydui.graphics;
export import cydui.components.base;

namespace cyd::ui::components {
  export class component_renderer_t {
  public:
    using sptr = std::shared_ptr<component_renderer_t>;

    static sptr make() {
      return std::make_shared<component_renderer_t>();
    }

    fabric::wiring::output_signal<component_renderer_t, compositing::compositing_node_t*> compositing_signal{};
  public:

    struct render_data_t {
      compositing::compositing_node_t compositing_node_{};
      bool graphics_dirty_ = true;
      bool compositing_dirty_ = true;
    };

    void queue_render(const component_base_t::sptr& component) {
      auto& data = component->get_data<render_data_t>();
      data.graphics_dirty_ = true;
    }

    void render(graphics::window_t& window, const component_base_t::sptr &component) {
      ZoneScopedN("Render Flow");
      if (is_compositing.test_and_set()) {
        composite_is_outdated.test_and_set();
        return;
      }
      update_fragments(component);
      {
        ZoneScopedN("Start Render");

        Application::run([](component_renderer_t* self, graphics::window_t* w, components::component_base_t *component_) {
          ZoneScopedN("Start layout render");
          self->start_render(component_, w);
        }, this, &window, component.get());
      }
      bool needs_compositing = false;
      {
        ZoneScopedN("Render");

        needs_compositing = repaint(component.get(), &window);
      }
      if (needs_compositing) {
        ZoneScopedN("Queuing Composition");
        // compositing_tree->fix_dimensions();
        Application::run_async(
          [](
            component_renderer_t*              self,
            std::atomic_flag*                  completion_flag,
            std::atomic_flag*                  is_outdated,
            graphics::window_t*                w,
            components::component_base_t::sptr root_ptr
          ) {
            ZoneScopedN("Compositing Layout");
            auto&& [root_node, must_recompose] = self->compose(root_ptr.get(), w);

            if (must_recompose) {
              ZoneScopedN("Compositing Frame");
              self->compositing_signal.emit(root_node);
            }

            completion_flag->clear();
            if (is_outdated->test()) {
              //-IMPORTANT: Need to make copy of pointers so they aren't passed
              // as references which will not survive
              component_renderer_t& s    = *self;
              graphics::window_t&   ww   = *w;
              std::atomic_flag&     flag = *is_outdated;
              //---------------------------------------------------------------
              w->bus->coroutine_enqueue(
                [](
                  component_renderer_t*              selff,
                  std::atomic_flag*                  is_outdated,
                  graphics::window_t*                w_,
                  components::component_base_t::sptr root_ptr_
                ) -> fabric::async::async<bool> {
                  selff->render(*w_, root_ptr_);
                  is_outdated->clear();
                  co_return true;
                },
                &s,
                &flag,
                &ww,
                root_ptr
              );
            }
          },
          this,
          &is_compositing,
          &composite_is_outdated,
          &window,
          component
        );
      }
    }

    void compose_all(graphics::window_t& window, const component_base_t::sptr &root_component) {
      ZoneScopedN("Compose All");
      if (is_compositing.test_and_set()) {
        composite_is_outdated.test_and_set();
        return;
      }

      Application::run_async(
        [](
          component_renderer_t*              self,
          std::atomic_flag*                  completion_flag,
          std::atomic_flag*                  is_outdated,
          graphics::window_t*                w,
          components::component_base_t::sptr root_ptr
        ) {
          ZoneScopedN("Compositing Layout");
          auto&& [root_node, must_recompose] = self->compose(root_ptr.get(), w);

          if (must_recompose) {
            ZoneScopedN("Compositing Frame");
            self->compositing_signal.emit(root_node);
          }

          completion_flag->clear();
          if (is_outdated->test()) {
            //-IMPORTANT: Need to make copy of pointers so they aren't passed
            // as references which will not survive
            component_renderer_t& s    = *self;
            graphics::window_t&   ww   = *w;
            std::atomic_flag&     flag = *is_outdated;
            //---------------------------------------------------------------
            w->bus->coroutine_enqueue(
              [](
                component_renderer_t*              selff,
                std::atomic_flag*                  is_outdated,
                graphics::window_t*                w_,
                components::component_base_t::sptr root_ptr_
              ) -> fabric::async::async<bool> {
                selff->render(*w_, root_ptr_);
                is_outdated->clear();
                co_return true;
              },
              &s,
              &flag,
              &ww,
              root_ptr
            );
          }
        },
        this,
        &is_compositing,
        &composite_is_outdated,
        &window,
        root_component
      );
    }

    void repaint_component(const component_base_t::sptr &component) {
      ZoneScopedN("Repaint Component");
      auto* parent = component->parent.has_value()
                       ? &(component->parent.value()->get_data<render_data_t>().compositing_node_)
                       : nullptr;
      queue_render(component);
      update_fragment(component.get(), parent);
    }

    bool render_all(graphics::window_t& win, const component_base_t::sptr &root) {
      ZoneScopedN("Render All");
      if (is_compositing.test()) {
        return false;
      }

      {
        ZoneScopedN("Start Render");

        Application::run([](component_renderer_t* self, graphics::window_t* w, components::component_base_t *component_) {
          ZoneScopedN("Start render");
          self->start_render(component_, w);
        }, this, &win, root.get());
      }

      bool needs_compositing = false;
      {
        ZoneScopedN("Render");

        needs_compositing = repaint(root.get(), &win);
      }

      return needs_compositing;
    }

  private:
    void update_fragments(const component_base_t::sptr& component) {
      ZoneScopedN("Fragments");
      auto* parent = component->parent.has_value()
                       ? &(component->parent.value()->get_data<render_data_t>().compositing_node_)
                       : nullptr;
      update_fragment(component.get(), parent);

      for (auto& child : component->children) {
        update_fragments(child);
      }
    }

    bool update_compositing_operation(component_base_t* component, compositing::compositing_node_t *parent_node) {
      ZoneScopedN("Update Compose Op");
      static auto get_num_value = [](const auto& it) -> auto {
        return dimensions::get_value(it).template as<dimensions::screen::pixel>().value;
      };

      auto& data = component->get_data<render_data_t>();

      int old_w = data.compositing_node_.op.w;
      int old_h = data.compositing_node_.op.h;

      auto* at = component->attrs();
      data.compositing_node_.id = (unsigned long)(component->state().get());
      data.compositing_node_.op = {
        .x       = static_cast<int>(get_num_value(at->_x) + get_num_value(at->_margin_left)),
        .y       = static_cast<int>(get_num_value(at->_y) + get_num_value(at->_margin_top)),
        .orig_x  = static_cast<int>(get_num_value(at->_padding_left)),
        .orig_y  = static_cast<int>(get_num_value(at->_padding_top)),
        .w       = static_cast<int>(get_num_value(at->_width)),
        .h       = static_cast<int>(get_num_value(at->_height)),
        .rot     = at->_rotation,   // dim->rot.val(),
        .scale_x = 1.0,             // dim->scale_x.val(),
        .scale_y = 1.0,             // dim->scale_y.val(),
        .animated = component->state()->is_animated(),
      };

      data.compositing_node_.set_parent(parent_node);

      return old_w != data.compositing_node_.op.w or old_h != data.compositing_node_.op.h;
    }

    void update_fragment(component_base_t* component, compositing::compositing_node_t *parent_node) {
      ZoneScopedN("Update Fragment");
      auto& data = component->get_data<render_data_t>();

      if (update_compositing_operation(component, parent_node)) {
        data.graphics_dirty_ = true;
      }

      if (data.graphics_dirty_) {
        data.compositing_node_.mark_flattening_target_dirty();
        paint_fragment(component, data.compositing_node_);
      }
    }

    void paint_fragment(
      component_base_t* component,
      compositing::compositing_node_t &compositing_node
    ) {
      auto get_num_value = [](const auto &it) -> auto {
        return dimensions::get_value(it).template as<dimensions::screen::pixel>().value;
      };

      auto &fragment = compositing_node.graphics;
      fragment.clear();

      auto* at = component->attrs();
      int half_top_border    = at->_border_width_top >> 1;
      int half_bottom_border = (at->_border_width_bottom >> 1) + (at->_border_width_bottom & 1);
      int half_left_border   = at->_border_width_left >> 1;
      int half_right_border  = (at->_border_width_right >> 1) + (at->_border_width_right & 1);

      // The four border corners, in reading order (left -> right, top ->bottom)
      int x1 = (half_left_border) - get_num_value(at->_padding_left);
      int y1 = (half_top_border) - get_num_value(at->_padding_top);

      int x2 = x1 + get_num_value(at->_width) - (half_left_border) - (half_right_border);
      int y2 = y1;

      int x3 = x1;
      int y3 = y1 + get_num_value(at->_height) - (half_top_border) - (half_bottom_border);

      int x4 = x2;
      int y4 = y3;

      fragment.draw<vg::rect>()
              .x(-dimensions::get_value(at->_padding_left))
              .y(-dimensions::get_value(at->_padding_top))
              .w(dimensions::get_value(at->_width))
              .h(dimensions::get_value(at->_height))
              .fill(component->get_style().background);
      fragment.draw<vg::line>()
              .x1(x1 - half_left_border).y1(y1)
              .x2(x2 + half_right_border).y2(y2)
              .stroke(at->_border_top)
              .stroke_width(at->_border_width_top)
              .stroke_dasharray(at->_border_dasharray_top);
      fragment.draw<vg::line>()
              .x1(x4 + half_right_border).y1(y4)
              .x2(x3 - half_left_border).y2(y3)
              .stroke(at->_border_bottom)
              .stroke_width(at->_border_width_bottom)
              .stroke_dasharray(at->_border_dasharray_bottom);
      fragment.draw<vg::line>()
              .x1(x3).y1(y3 + half_bottom_border)
              .x2(x1).y2(y1 - half_top_border)
              .stroke(at->_border_left)
              .stroke_width(at->_border_width_left)
              .stroke_dasharray(at->_border_dasharray_left);
      fragment.draw<vg::line>()
              .x1(x2).y1(y2 - half_top_border)
              .x2(x4).y2(y4 + half_bottom_border)
              .stroke(at->_border_right)
              .stroke_width(at->_border_width_right)
              .stroke_dasharray(at->_border_dasharray_right);


      component->get_event_dispatcher()->paint_fragment(fragment);

      //! Grow fragment if needed
      // if (!fragment.empty()) {
      //   for (const auto& elem: fragment.elements) {
      //     auto fp = elem->get_footprint();
      //     if ((fp.x + fp.w) > compositing_node.op.w) {
      //       compositing_node.op.w = fp.x.value_as_base_unit() + fp.w.value_as_base_unit();
      //     }
      //     if ((fp.y + fp.h) > compositing_node.op.h) {
      //       compositing_node.op.h = fp.y.value_as_base_unit() + fp.h.value_as_base_unit();
      //     }
      //   }
      // }
    }

    void start_render(component_base_t* component, graphics::window_t* render_target) {
      auto& data = component->get_data<render_data_t>();
      if (data.graphics_dirty_ or data.compositing_node_.is_flattened_node()) {
        data.compositing_node_.start_render(render_target);
      }

      for (auto& child : component->children) {
        start_render(child.get(), render_target);
      }
    }

    bool repaint(component_base_t* component, graphics::window_t* render_target) {
      auto& data = component->get_data<render_data_t>();
      if (data.graphics_dirty_ or data.compositing_node_.is_dirty_from_flattening()) {
        data.graphics_dirty_ = false;
        data.compositing_dirty_ = true;
        data.compositing_node_.render(render_target);
      }

      for (auto& child : component->children) {
        if (repaint(child.get(), render_target)) {
          data.compositing_dirty_ = true;
        }
      }
      return data.compositing_dirty_;
    }

    std::pair<compositing::compositing_node_t*, bool> compose(component_base_t* component, graphics::window_t* render_target) {
      auto& data = component->get_data<render_data_t>();
      // compositing_node_.clear_composite_texture(render_target);
      bool did_compositing = false;
      if (data.compositing_dirty_) {
        data.compositing_dirty_ = false;

        data.compositing_node_.compose_own(render_target);

        for (auto &child: component->children) {
          auto&& [node, _] = compose(child.get(), render_target);
          if (nullptr != node) {
            data.compositing_node_.compose(render_target, node);
          }
        }

        did_compositing = true;
      }

      if (data.compositing_node_.is_flattened_node()) {
        return {nullptr, did_compositing};
      } else {
        return {&data.compositing_node_, did_compositing};
      }
    }

  private:
    std::atomic_flag is_compositing {false};
    std::atomic_flag composite_is_outdated {false};
  };
}