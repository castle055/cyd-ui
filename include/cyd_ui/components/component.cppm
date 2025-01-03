// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components;

import std;

import fabric.logging;
import fabric.memory.lazy_alloc;
import fabric.async;

export import :attributes;
export import :event_handler;
export import :with_specialization;

export namespace cyd::ui::components {
  struct component_base_t;
  template<ComponentEventHandlerConcept EVH, typename T>
  struct component_t;
  struct component_state_t;
  using component_state_ref = std::shared_ptr<component_state_t>;
}


namespace cyd::ui::components {
  struct component_state_t {
    virtual ~component_state_t() = default;

    std::shared_ptr<fabric::async::async_bus_t> window = nullptr;

    std::unordered_map<std::string, component_state_ref> children_states { };

    component_state_t() = default;
    explicit component_state_t(void* props) { }

    bool _dirty = false;

    bool focused = false;
    bool hovering = false;

    std::optional<std::shared_ptr<component_base_t>> component_instance = std::nullopt;

    /*!
     * @brief Marks this component state as needing to be redrawn.
     *
     * Events declared in `cydui_events.h` are dispatched automatically
     * by the layout, which then checks for any `dirty` components that need
     * redrawing.
     *
     * It is due diligence of the component developer to appropriately mark
     * the component as dirty when appropriate. As a baseline, any mutation
     * to the state of a component should mark it dirty if said mutation
     * affects how the component, or its children, are rendered.
     */
    void mark_dirty() {
      _dirty = true;
    }

    template<fabric::async::EventType EV>
    void emit(EV ev) {
      if (nullptr == this->window) return;
      this->window->emit<EV>(ev);
    }

    /*!
     * @brief Emits a RedrawEvent for this component.
     *
     * @warning This is only meant to be called from custom event handlers.
     * Event handlers for events declared in `cydui_events.h`
     * automatically trigger a redraw if `state->mark_dirty()` gets called.
     * Use that instead.
     */
    void force_redraw() {
      emit<RedrawEvent>({.component = this});
    }

    std::string component_name() const {
      return component_name_;
    }

    component_state_t* parent() {
      return parent_;
    }
  private:
    friend struct component_base_t;
    template<ComponentEventHandlerConcept EVH, typename T>
    friend struct component_t;

    void add_children_state(const std::string& id, const component_state_ref &child) {
      children_states[id] = child;
      child->parent_ = this;
      child->window = window;
    }

    void set_component_name(const std::string& name) {
      component_name_ = name;
    }

    std::string component_name_;
    component_state_t* parent_;
  };

  struct component_base_t {
    using sptr = std::shared_ptr<component_base_t>;

    virtual ~component_base_t() = default; //{
    //  ! All of this is done in the `component_t` class destructor
    //  clear_subscribed_listeners();
    //  if (state.has_value()) {
    //    state.value()->component_instance = std::nullopt;
    //  }
    //}
    virtual void clear_children() = 0;
    virtual attrs_component<>* attrs() = 0;
    virtual void* get_props() = 0;
    virtual std::string name() = 0;

    virtual std::shared_ptr<event_handler_t> event_handler() = 0;
    virtual void dispatch_key_press(const KeyEvent& ev) = 0;
    virtual void dispatch_key_release(const KeyEvent& ev) = 0;
    virtual void dispatch_button_press(const Button& ev, dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_button_release(const Button& ev, dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_scroll(dimension_t::value_type dx, dimension_t::value_type dy) = 0;
    virtual void dispatch_mouse_enter(dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_mouse_exit(dimension_t::value_type x, dimension_t::value_type y) = 0;
    virtual void dispatch_mouse_motion(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual void redraw() = 0;
    virtual void
    get_fragment(
      std::unique_ptr<cyd::ui::compositing::compositing_node_t> &compositing_node
    ) = 0;

    virtual std::shared_ptr<component_state_t> create_state_instance() = 0;

    virtual component_base_t* find_by_coords(dimension_t::value_type x, dimension_t::value_type y) = 0;

    virtual attrs_dimensions<>& get_dimensional_relations() = 0;
    virtual std::shared_ptr<dimension_ctx_t> get_dimensional_context() = 0;

    internal_relations_t& get_internal_relations() {
      return internal_relations;
    }

  public:
    component_state_ref state() const {
      if (state_.has_value()) {
        return state_.value().lock();
      } else {
        return {nullptr};
      }
    }

    virtual void configure_event_handler() = 0;

  protected:
    void set_state(const component_state_ref& state) {
      state_ = state;
    }

    /**
     * \brief Update props and attributes with other component
     * \param other
     * \return [bool] true if we need to redraw
     */
    virtual bool update_with(std::shared_ptr<component_base_t> other) = 0;

    template<ComponentEventHandlerConcept EVH, typename T>
    friend struct component_t;

    internal_relations_t internal_relations{};

  public:
    std::optional<component_base_t*> parent = std::nullopt;
    std::list<std::shared_ptr<component_base_t>> children{};
  private:
    std::optional<std::weak_ptr<component_state_t>> state_ = std::nullopt;
  };


  template<ComponentEventHandlerConcept EVH, typename T>
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
        std::memcpy(&props(), &other_component->props(), sizeof(typename T::props_t));
        dirty   = true;
      }
      if (not (*attrs() == *(other_component->attrs()))) {
        attrs()->update_with(*(other_component->attrs()));
        dirty    = true;
      }

      return dirty;
    }

    void mount_child(
      const std::string&                            id,
      std::shared_ptr<component_base_t>             child,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove
    ) {
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
        std::list<std::shared_ptr<component_base_t>>::iterator iter = std::prev(children.end());

        // Configure event handler
        child->configure_event_handler();

        // Configure dimensional context
        auto& ctx = *child->get_dimensional_context();
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

        ctx.set_parameter("prev_x", [this, iter] {
          if (iter == this->children.begin()) {
            return dimension_t {0_px};
          } else {
            return (*std::prev(iter))->get_dimensional_relations()._x;
          }
        });
        ctx.set_parameter("prev_y", [this, iter] {
          if (iter == this->children.begin()) {
            return dimension_t {0_px};
          } else {
            return (*std::prev(iter))->get_dimensional_relations()._y;
          }
        });
        ctx.set_parameter("prev_width", [this, iter] {
          if (iter == this->children.begin()) {
            return dimension_t {0_px};
          } else {
            return (*std::prev(iter))->get_dimensional_relations()._width;
          }
        });
        ctx.set_parameter("prev_height", [this, iter] {
          if (iter == this->children.begin()) {
            return dimension_t {0_px};
          } else {
            return (*std::prev(iter))->get_dimensional_relations()._height;
          }
        });

        // Redraw child
        pending_redraw.push_back(child);
      }
    }

    template <typename C>
    void add_children(
      std::string                                   prefix_id,
      const std::vector<C>&                         children_to_add,
      std::list<std::shared_ptr<component_base_t>>& pending_redraw,
      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>& pending_remove
    ) {
      std::size_t id_i = 0;
      for (auto& item: children_to_add) {
        for (auto& component_pair: item.get_components()) {
          auto [id_, component] = component_pair;
          std::string id        = prefix_id;
          id.append(std::to_string(id_i));
          id.append(":");
          id.append(id_);

          mount_child(id, component, pending_redraw, pending_remove);
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

      std::unordered_map<
        std::shared_ptr<component_base_t>,
        std::list<std::shared_ptr<component_base_t>>::iterator>
                                                   pending_remove{};
      std::list<std::shared_ptr<component_base_t>> pending_redraw{};
      for (auto it = children.begin(); it != children.end(); ++it) {
        pending_remove.emplace(*it, it);
      }

      std::vector<component_builder_t>& content_children = this->_content;
      add_children("content:", content_children, pending_redraw, pending_remove);

      std::vector<component_holder_t> new_children = event_handler_->on_redraw(
        this->_x,
        this->_y,
        this->internal_relations.cw,
        this->internal_relations.ch,
        this->_padding_top,
        this->_padding_bottom,
        this->_padding_left,
        this->_padding_right
      );
      add_children("", std::move(new_children), pending_redraw, pending_remove);

      for (const auto& remove: pending_remove) {
        unmount_child(remove.second);
      }
      for (const auto& child: pending_redraw) {
        // Redraw children
        child->redraw();
      }
    }

    void get_fragment(std::unique_ptr<cyd::ui::compositing::compositing_node_t>& compositing_node
    ) override {
      for (auto& child: children) {
        compositing_node->children.emplace_back(new compositing::compositing_node_t{});
        child->get_fragment(compositing_node->children.back());
      }

      auto get_num_value = [](const auto& it) -> auto {
        return dimensions::get_value(it).template as<dimensions::screen::pixel>().value;
      };

      compositing_node->id = (unsigned long)(this->state().get());
      compositing_node->op = {
        .x       = get_num_value(this->_x) + get_num_value(this->_margin_left),
        .y       = get_num_value(this->_y) + get_num_value(this->_margin_top),
        .orig_x  = get_num_value(this->_padding_left),
        .orig_y  = get_num_value(this->_padding_top),
        .w       = get_num_value(this->_width),
        .h       = get_num_value(this->_height),
        .rot     = this->_rotation, // dim->rot.val(),
        .scale_x = 1.0,             // dim->scale_x.val(),
        .scale_y = 1.0,             // dim->scale_y.val(),
      };

      auto& fragment = compositing_node->graphics;
      fragment.clear();

      fragment.draw<vg::rect>()
        .x(-dimensions::get_value(this->_padding_left))
        .y(-dimensions::get_value(this->_padding_top))
        .w(dimensions::get_value(this->_width))
        .h(dimensions::get_value(this->_height))
        .fill(this->_background);
      fragment.draw<vg::line>()
        .x1(-dimensions::get_value(this->_padding_left))
        .y1(-dimensions::get_value(this->_padding_top))
        .x2(-dimensions::get_value(this->_padding_left) + dimensions::get_value(this->_width))
        .y2(-dimensions::get_value(this->_padding_top))
        .stroke(this->_border_top)
        .stroke_width(this->_border_width_top)
        .stroke_dasharray(this->_border_dasharray_top);
      fragment.draw<vg::line>()
        .x1(-dimensions::get_value(this->_padding_left) + dimensions::get_value(this->_width))
        .y1(-dimensions::get_value(this->_padding_top) + dimensions::get_value(this->_height))
        .x2(-dimensions::get_value(this->_padding_left))
        .y2(-dimensions::get_value(this->_padding_top) + dimensions::get_value(this->_height))
        .stroke(this->_border_bottom)
        .stroke_width(this->_border_width_bottom)
        .stroke_dasharray(this->_border_dasharray_bottom);
      fragment.draw<vg::line>()
        .x1(-dimensions::get_value(this->_padding_left))
        .y1(-dimensions::get_value(this->_padding_top) + dimensions::get_value(this->_height))
        .x2(-dimensions::get_value(this->_padding_left))
        .y2(-dimensions::get_value(this->_padding_top))
        .stroke(this->_border_left)
        .stroke_width(this->_border_width_left)
        .stroke_dasharray(this->_border_dasharray_left);
      fragment.draw<vg::line>()
        .x1(-dimensions::get_value(this->_padding_left) + dimensions::get_value(this->_width))
        .y1(-dimensions::get_value(this->_padding_top))
        .x2(-dimensions::get_value(this->_padding_left) + dimensions::get_value(this->_width))
        .y2(-dimensions::get_value(this->_padding_top) + dimensions::get_value(this->_height))
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
          if (fp.x + fp.w > compositing_node->op.w) {
            compositing_node->op.w = fp.x + fp.w;
          }
          if (fp.y + fp.h > compositing_node->op.h) {
            compositing_node->op.h = fp.y + fp.h;
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
