// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components;

import std;

import fabric.logging;
import fabric.memory.lazy_alloc;
import fabric.async;
export import fabric.wiring.signals;

export import :attributes;
export import :event_handler;
export import :with_specialization;
export import :state;
export import :base;

export using children_list = std::vector<cyd::ui::components::component_holder_t>;

namespace cyd::ui::components {
  export template<ComponentEventHandlerConcept EVH, typename T>
  struct component_t:
    public component_base_t,
    public attrs_component<T> {
  private:
    auto& props() {
      return (dynamic_cast<T*>(this)->props);
    }

    bool update_with(std::shared_ptr<component_base_t> other) override {
      auto other_component = std::dynamic_pointer_cast<component_t>(other);
      if (!other_component) {
        LOG::print{ERROR
        }("Attempted to update component of type ({}) with type ({})", this->name(), other->name());
        return false;
      }

      bool dirty = false;
      if (not refl::deep_eq(props(), other_component->props())) {
        props() = other_component->props();
        // std::memcpy(&props(), &other_component->props(), sizeof(typename T::props_t));
        dirty = true;
      }
      if (not (*attrs() == *(other_component->attrs()))) {
        attrs()->update_with(*(other_component->attrs()));
        dirty = true;
      }

      if (update_fields(other_component)) {
        dirty = true;
      }

      return dirty;
    }

    bool update_fields(std::shared_ptr<component_t>& other) {
      bool dirty = false;
      [&]<std::size_t ...I>(std::index_sequence<I...>) {
        (update_field<I>(dirty, other), ...);
      }(std::make_index_sequence<refl::field_count<T>>());
      return dirty;
    }

    template <std::size_t I>
    void update_field(bool& dirty, std::shared_ptr<component_t>& other_) {
      auto other = std::dynamic_pointer_cast<T>(other_);
      using field = refl::field<T, I>;
      using field_type = typename field::type;

      if constexpr (packtl::is_type<fabric::wiring::signal, field_type>::value) {
        auto& this_signal = field::from_instance(*dynamic_cast<T*>(this));
        auto& other_signal = field::from_instance(*other);

        if (this_signal != other_signal) {
          this_signal = other_signal;
          dirty = true;
        }
      }
    }

    std::shared_ptr<component_base_t> mount_child(
      const std::string&                            id,
      std::shared_ptr<component_base_t>             child,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove,
      std::optional<std::shared_ptr<component_base_t>> prev
    ) {
      std::shared_ptr<component_base_t> mounted_child {child};
      // Get or Create state for component
      component_state_ref child_state;
      if (state()->children_states.contains(id)) {
        child_state = state()->children_states[id];
      } else {
        child_state = child->create_state_instance();
        state()->add_children_state(id, child_state);
      }

      if (child_state->component_instance.has_value()) {
        pending_remove.erase(child_state->component_instance.value());
        mounted_child = child_state->component_instance.value();

        // Redraw child
        if (child_state->component_instance.value()->update_with(child)) {
          pending_redraw.push_back(child_state->component_instance.value());
        }
      } else {
        // Set child's variables
        child->parent                = this;
        attrs_component<>* c_attrs   = child->attrs();
        child->internal_relations.cx = this->internal_relations.cx + c_attrs->_x +
                                       c_attrs->_margin_left + c_attrs->_padding_left;
        child->internal_relations.cy =
          this->internal_relations.cy + c_attrs->_y + c_attrs->_margin_top + c_attrs->_padding_top;
        child->set_state(child_state);
        child_state->component_instance = child;
        children.push_back(child);

        // Configure event handler
        child->configure_event_handler();

        // Configure dimensional context
        auto& ctx = *child->get_dimensional_context();
        ctx.set_parameter("self_x", [child] {
          return child->get_internal_relations().cx;
        });
        ctx.set_parameter("self_y", [child] {
          return child->get_internal_relations().cy;
        });
        ctx.set_parameter("self_width", [child] {
          return child->get_internal_relations().cw;
        });
        ctx.set_parameter("self_height", [child] {
          return child->get_internal_relations().ch;
        });

        ctx.set_parameter("parent_x", [child] {
          return child->parent.value()->get_internal_relations().cx;
        });
        ctx.set_parameter("parent_y", [child] {
          return child->parent.value()->get_internal_relations().cy;
        });
        ctx.set_parameter("parent_width", [child] {
          return child->parent.value()->get_internal_relations().cw;
        });
        ctx.set_parameter("parent_height", [child] {
          return child->parent.value()->get_internal_relations().ch;
        });

        ctx.set_parameter("prev_x", [=] {
          if (prev.has_value()) {
            return prev.value()->get_dimensional_relations()._x;
          }
          return dimension_t {0_px};
        });
        ctx.set_parameter("prev_y", [=] {
          if (prev.has_value()) {
            return prev.value()->get_dimensional_relations()._y;
          }
          return dimension_t {0_px};
        });
        ctx.set_parameter("prev_width", [=] {
          if (prev.has_value()) {
            return prev.value()->get_dimensional_relations()._width;
          }
          return dimension_t {0_px};
        });
        ctx.set_parameter("prev_height", [=] {
          if (prev.has_value()) {
            return prev.value()->get_dimensional_relations()._height;
          }
          return dimension_t {0_px};
        });

        // Redraw child
        pending_redraw.push_back(child);
      }

      return mounted_child;
    }

    void add_children(
      std::vector<component_holder_t> &             children_to_add,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove
    ) {
      std::size_t id_i = 0;
      std::optional<std::shared_ptr<component_base_t>> prev{std::nullopt};
      for (auto& item: children_to_add) {
        for (auto& component_pair: item.get_components()) {
          auto [id_, component] = component_pair;
          std::string id        = std::format("{}:{}", id_i, id_);

          auto mounted_child = mount_child(id, component, pending_redraw, pending_remove, prev);
          prev.reset();
          prev.emplace(mounted_child);
        }
        ++id_i;
      }
    }

    void unmount_child(const std::list<std::shared_ptr<component_base_t>>::iterator& child) {
      (*child)->get_dimensional_context()->clear_parameters();

      // Delete event handler, this component will now stop reacting to events
      (*child)->event_handler().reset();

      (*child)->state()->component_instance = std::nullopt;

      children.erase(child);
    }

  public:
    component_t() {
      internal_relations.cx = this->_x + this->_margin_left + this->_padding_left;
      internal_relations.cy = this->_y + this->_margin_top + this->_padding_top;
    }

    ~component_t() override {
      children.clear();
      if (state()) {
        state()->component_instance = std::nullopt;
      }
    };

  private:
    void configure_event_handler() override {
      event_handler_ = std::make_shared<EVH>(event_handler_data_t<T>{
        *static_cast<T*>(this),
        parent.has_value()? (parent.value()->event_handler().get()): nullptr,
        children,
        std::dynamic_pointer_cast<typename T::state_t>(state()),
        (std::dynamic_pointer_cast<typename T::state_t>(state()))->window,
        static_cast<T*>(this)->props,
        *static_cast<attrs_component<T>*>(this),
      });

      if constexpr (refl::Reflected<EVH>) {
        static constexpr std::size_t field_count = refl::field_count<EVH>;

        configure_event_handler_fields(std::make_index_sequence<field_count> { });
      }
    }

    template <std::size_t... I>
    void configure_event_handler_fields(std::index_sequence<I...>) {
        (configure_event_handler_field<I>(), ...);
    }

    template <std::size_t FieldI>
    void configure_event_handler_field() {
      using field = refl::field<EVH, FieldI>;
      using field_type = typename field::type;

      if constexpr (packtl::is_type<use_context, field_type>::value) {
        using context_type = typename field_type::context_type;

        use_context<context_type>& ctx_ref = field::from_instance(*event_handler_);

        auto ctx = find_context<context_type>();

        if (ctx.has_value()) {
          ctx_ref.ctx = ctx.value();
        } else {
          ctx_ref.ctx = new context_type{};
          ctx_ref.owns_context = true;
        }

        ctx_ref.state = state_.value().lock();
        ctx_ref.start_listening();
      } else if constexpr (packtl::is_type<provide_context, field_type>::value) {
        using context_type = typename field_type::context_type;

        provide_context<context_type>& ctx_ref = field::from_instance(*event_handler_);
        ctx_ref.state = state_.value().lock();
      }
    }
  public:
    void clear_children() override {
      children.clear();
    }
    attrs_component<>* attrs() override {
      // Yes, the order of casting matters here because a conversion from `this` to
      // `(attrs_component<>*)` does not work since that type is not a base of this
      // class. So we need to cast to the base class first and then to its `void`
      // specialization.
      return reinterpret_cast<attrs_component<>*>(static_cast<attrs_component<T>*>(this));
    }

    std::shared_ptr<component_state_t> create_state_instance() override {
      std::shared_ptr<component_state_t> state;
      if constexpr (requires { new typename T::state_t{std::declval<typename T::props_t*>()}; }) {
        state = std::shared_ptr<component_state_t>{
          new typename T::state_t(static_cast<typename T::props_t*>(get_props()))
        };
      } else {
        state = std::shared_ptr<component_state_t>{new typename T::state_t()};
      }
      state->set_component_name(this->name());
      set_state(state);
      return state;
    }

    std::shared_ptr<event_handler_t> event_handler() override {
      return std::dynamic_pointer_cast<event_handler_t>(event_handler_);
    }

    void redraw() override {
      state()->_dirty = false;
      graphics_dirty_ = true;

      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>
                                                   pending_remove{};
      std::list<std::shared_ptr<component_base_t>> pending_redraw{};
      for (auto it = children.begin(); it != children.end(); ++it) {
        pending_remove.emplace(*it, it);
      }

      component_builder_t content_children_builder{}; {
        std::vector<component_builder_t> &content_children = this->_content;
        std::size_t id_i                                   = 0;
        for (auto &item: content_children) {
          for (auto &component_pair: item.get_component_constructors()) {
            auto [id_, component] = component_pair;
            const std::string id  = std::format("content:{}:{}", id_i, id_);

            content_children_builder.append_component(id, component);
          }
          ++id_i;
        }
      }

      std::vector<component_holder_t> new_children = event_handler_->on_redraw(
        this->_x,
        this->_y,
        this->internal_relations.cw,
        this->internal_relations.ch,
        this->_padding_top,
        this->_padding_bottom,
        this->_padding_left,
        this->_padding_right,
        content_children_builder
      );

      add_children(new_children, pending_redraw, pending_remove);

      for (const auto& remove: pending_remove) {
        unmount_child(remove.second);
      }
      for (const auto& child: pending_redraw) {
        // Redraw children
        child->redraw();
      }
    }

    void get_fragment(cyd::ui::compositing::compositing_node_t& compositing_node
    ) override {
      // for (auto& child: children) {
      //   compositing_node.children.emplace_back(new compositing::compositing_node_t{});
      //   child->get_fragment(compositing_node.children.back());
      // }
      //
      auto get_num_value = [](const auto& it) -> auto {
        return dimensions::get_value(it).template as<dimensions::screen::pixel>().value;
      };

      compositing_node.id = (unsigned long)(this->state().get());
      compositing_node.op = {
        .x       = static_cast<int>(get_num_value(this->_x) + get_num_value(this->_margin_left)),
        .y       = static_cast<int>(get_num_value(this->_y) + get_num_value(this->_margin_top)),
        .orig_x  = static_cast<int>(get_num_value(this->_padding_left)),
        .orig_y  = static_cast<int>(get_num_value(this->_padding_top)),
        .w       = static_cast<int>(get_num_value(this->_width)),
        .h       = static_cast<int>(get_num_value(this->_height)),
        .rot     = this->_rotation, // dim->rot.val(),
        .scale_x = 1.0,             // dim->scale_x.val(),
        .scale_y = 1.0,             // dim->scale_y.val(),
      };

      auto& fragment = compositing_node.graphics;
      fragment.clear();

      int half_top_border    = this->_border_width_top >> 1;
      int half_bottom_border = (this->_border_width_bottom >> 1) + (this->_border_width_bottom & 1);
      int half_left_border   = this->_border_width_left >> 1;
      int half_right_border  = (this->_border_width_right >> 1) + (this->_border_width_right & 1);

      // The four border corners, in reading order (left -> right, top ->bottom)
      int x1 = (half_left_border) - get_num_value(this->_padding_left);
      int y1 = (half_top_border) - get_num_value(this->_padding_top);

      int x2 = x1 + get_num_value(this->_width) - (half_left_border) - (half_right_border);
      int y2 = y1;

      int x3 = x1;
      int y3 = y1 + get_num_value(this->_height) - (half_top_border) - (half_bottom_border);

      int x4 = x2;
      int y4 = y3;

      fragment.draw<vg::rect>()
              .x(-dimensions::get_value(this->_padding_left))
              .y(-dimensions::get_value(this->_padding_top))
              .w(dimensions::get_value(this->_width))
              .h(dimensions::get_value(this->_height))
              .fill(this->_background);
      fragment.draw<vg::line>()
              .x1(x1 - half_left_border).y1(y1)
              .x2(x2 + half_right_border).y2(y2)
              .stroke(this->_border_top)
              .stroke_width(this->_border_width_top)
              .stroke_dasharray(this->_border_dasharray_top);
      fragment.draw<vg::line>()
              .x1(x4 + half_right_border).y1(y4)
              .x2(x3 - half_left_border).y2(y3)
              .stroke(this->_border_bottom)
              .stroke_width(this->_border_width_bottom)
              .stroke_dasharray(this->_border_dasharray_bottom);
      fragment.draw<vg::line>()
              .x1(x3).y1(y3 + half_bottom_border)
              .x2(x1).y2(y1 - half_top_border)
              .stroke(this->_border_left)
              .stroke_width(this->_border_width_left)
              .stroke_dasharray(this->_border_dasharray_left);
      fragment.draw<vg::line>()
              .x1(x2).y1(y2 - half_top_border)
              .x2(x4).y2(y4 + half_bottom_border)
              .stroke(this->_border_right)
              .stroke_width(this->_border_width_right)
              .stroke_dasharray(this->_border_dasharray_right);


      event_handler_->draw_fragment(
        fragment,
        dimensions::get_value(this->internal_relations.cx),
        dimensions::get_value(this->internal_relations.cy),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
      if (!fragment.empty()) {
        for (const auto& elem: fragment.elements) {
          auto fp = elem->get_footprint();
          if (fp.x + fp.w > compositing_node.op.w) {
            compositing_node.op.w = fp.x.value_as_base_unit() + fp.w.value_as_base_unit();
          }
          if (fp.y + fp.h > compositing_node.op.h) {
            compositing_node.op.h = fp.y.value_as_base_unit() + fp.h.value_as_base_unit();
          }
        }
      }
    }

    component_base_t*
    find_by_coords(dimension_t::value_type x, dimension_t::value_type y) override {
      using namespace dimensions;

      component_base_t* found = nullptr;
      for (auto c = children.rbegin(); c != children.rend(); ++c) {
        auto cx = get_value(c->get()->attrs()->_x);
        auto cy = get_value(c->get()->attrs()->_y);
        auto mx = get_value(c->get()->attrs()->_margin_left);
        auto my = get_value(c->get()->attrs()->_margin_top);
        auto px = get_value(c->get()->attrs()->_padding_left);
        auto py = get_value(c->get()->attrs()->_padding_top);
        found = (*c)->find_by_coords(x - cx - mx - px, y - cy - my - py);
        if (nullptr != found) {
          return found;
        }
      }

      if (x < 0 ||
          x >= get_value(this->_width) ||
          y < 0 ||
          y >= get_value(this->_height)) {
        return nullptr;
      }
      return this;
    }

    attrs_dimensions<>& get_dimensional_relations() override {
      return *reinterpret_cast<attrs_dimensions<>*>(static_cast<attrs_dimensions<T>*>(this));
    }

    std::shared_ptr<dimension_ctx_t> get_dimensional_context() override {
      return this->dimension_ctx;
    }
  public:
    void dispatch_key_press(const KeyEvent& ev) final {
      event_handler_->on_key_press(
        ev,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_key_release(const KeyEvent& ev) final {
      event_handler_->on_key_release(
        ev,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_text_input(const TextInputEvent& ev) final {
      event_handler_->on_text_input(
        ev,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_button_press(
      const Button& button, dimension_t::value_type x, dimension_t::value_type y
    ) final {
      event_handler_->on_button_press(
        button,
        x,
        y,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_button_release(
      const Button& button, dimension_t::value_type x, dimension_t::value_type y
    ) final {
      event_handler_->on_button_release(
        button,
        x,
        y,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_mouse_enter(dimension_t::value_type x, dimension_t::value_type y) final {
      event_handler_->on_mouse_enter(
        x,
        y,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_mouse_exit(dimension_t::value_type x, dimension_t::value_type y) final {
      event_handler_->on_mouse_exit(
        x,
        y,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_mouse_motion(dimension_t::value_type x, dimension_t::value_type y) final {
      event_handler_->on_mouse_motion(
        x,
        y,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }
    void dispatch_scroll(dimension_t::value_type dx, dimension_t::value_type dy) final {
      event_handler_->on_scroll(
        dx,
        dy,
        dimensions::get_value(this->_x),
        dimensions::get_value(this->_y),
        dimensions::get_value(this->internal_relations.cw),
        dimensions::get_value(this->internal_relations.ch),
        dimensions::get_value(this->_padding_top),
        dimensions::get_value(this->_padding_bottom),
        dimensions::get_value(this->_padding_left),
        dimensions::get_value(this->_padding_right)
      );
    }

  private:
    std::shared_ptr<EVH> event_handler_{};
  };
}

export template<typename, typename = void>
constexpr bool is_type_complete_v = false;

export template<typename T>
constexpr bool is_type_complete_v
  <T, std::void_t<decltype(sizeof(T))>> = true;


