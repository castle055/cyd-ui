//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_H
#define CYD_UI_COMPONENT_H

#include "../cydstd/lazy_alloc.h"

#include "component_event_handler.h"
#include "with_specialization.h"

#include "component_attributes.h"
#include "component_macros.h"

#include <optional>

#include "window.h"

namespace cydui::layout {
    class Layout;
}

namespace cydui::components {
    struct component_state_t {
      window::CWindow* win;
      
      std::optional<component_state_t*> parent = std::nullopt;
      std::unordered_map<std::string, component_state_t*> children_states {};
      
      bool _dirty = false;
      
      bool focused = false;
      bool hovering = false;
      
      std::optional<component_base_t*> component_instance = std::nullopt;
      
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
      
      template<cydui::events::EventType T>
      void emit(typename T::DataType ev) {
        ev.win = get_id(this->win->win_ref);
        cydui::events::emit<T>(ev);
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
    };
    
    struct component_base_t {
      std::optional<component_state_t*> state = std::nullopt;
      std::optional<component_base_t*> parent = std::nullopt;
      std::vector<component_base_t*> children {};
      
      std::vector<cydui::events::listener_t> subscribed_listeners {};
      
      virtual ~component_base_t() {
        clear_subscribed_listeners();
        if (state.has_value()) {
          state.value()->component_instance = std::nullopt;
        }
      }
      virtual void subscribe_events() = 0;
      virtual void clear_children() = 0;
      
      virtual event_handler_t* event_handler() = 0;
      
      virtual void redraw(cydui::layout::Layout* layout) = 0;
      virtual void
      get_fragment(cydui::layout::Layout* layout, cydui::compositing::compositing_node_t* compositing_node) = 0;
      
      virtual component_state_t* create_state_instance() = 0;
      
      virtual component_base_t* find_by_coords(int x, int y) = 0;
      
      virtual component_dimensional_relations_t get_dimensional_relations() = 0;
    
    protected:
      void add_event_listeners(const std::unordered_map<std::string, event_handler_t::listener_data_t> &listeners) {
        for (auto &l: listeners) {
          auto &[ev_type, handler] = l;
          subscribed_listeners.push_back(cydui::events::on_event_raw(ev_type, handler.handler));
        }
      }
      void clear_subscribed_listeners() {
        for (auto &item: subscribed_listeners) {
          item.remove();
        }
        subscribed_listeners.clear();
      }
    };
    
    template<ComponentEventHandlerConcept EVH, typename T>
    struct component_t:
      public component_base_t,
      attrs_component<T> {
    private: //! Private attributes
      cydui::dimensions::dimension_t cx;
      cydui::dimensions::dimension_t cy;
      cydui::dimensions::dimension_t cw;
      cydui::dimensions::dimension_t ch;
    private:
      lazy_alloc<EVH> event_handler_ {};
    
    public:
      ~component_t() override = default;
      
      void subscribe_events() override {
        clear_subscribed_listeners();
        if (parent.has_value()) {
          event_handler_->parent = parent.value()->event_handler();
        } else {
          event_handler_->parent = nullptr;
        }
        EVH * evh = event_handler_.operator->();
        evh->state = (typename T::state_t*) state.value();
        evh->props = &(((T*) this)->props);
        evh->attrs = (attrs_component<T>*) this;
        evh->get_dim = [this] {return get_dimensional_relations();};
        add_event_listeners(evh->get_event_listeners());
      }
      void clear_children() override {
        for (auto &child: children) {
          delete child;
        }
        children.clear();
      }
      
      component_state_t* create_state_instance() override {
        return new typename T::state_t;
      }
      event_handler_t* event_handler() override {
        return event_handler_;
      }
      
      void redraw(cydui::layout::Layout* layout) override {
        std::vector<component_holder_t> new_children = this->_content();
        std::vector<component_holder_t> redraw_children = event_handler_->on_redraw();
        for (auto &item: redraw_children) {
          new_children.push_back(item);
        }
        
        std::size_t id_i = 0;
        for (auto &item: new_children) {
          for (auto &component_pair: item.get_components()) {
            auto [id_, component] = component_pair;
            std::string id = id_;
            id.append(":");
            id.append(std::to_string(id_i));
            
            // Get or Create state for component
            component_state_t* child_state;
            if (state.value()->children_states.contains(id)) {
              child_state = state.value()->children_states[id];
            } else {
              child_state = component->create_state_instance();
              state.value()->children_states[id] = child_state;
            }
            
            // Set child's variables
            component->state = child_state;
            child_state->win = state.value()->win;
            child_state->parent = state.value();
            child_state->component_instance = component;
            component->parent = this;
            children.push_back(component);
            
            // Subscribe child events
            component->subscribe_events();
            
            // Redraw child
            component->redraw(layout);
          }
          ++id_i;
        }
      }
      
      void
      get_fragment(cydui::layout::Layout* layout, cydui::compositing::compositing_node_t* compositing_node) override {
        for (auto &child: children) {
          auto* child_node = new cydui::compositing::compositing_node_t;
          compositing_node->children.push_back(child_node);
          child->get_fragment(layout, child_node);
        }
        
        compositing_node->id = (unsigned long) (this->state.value());
        compositing_node->op = {
          .x = this->_x.val(),
          .y = this->_y.val(),
          .orig_x = this->_margin_left.val() + this->_padding_left.val(),
          .orig_y = this->_margin_top.val() + this->_padding_top.val(),
          .w = this->_w.val(),
          .h = this->_h.val(),
          .rot = 0.0, // dim->rot.val(),
          .scale_x = 1.0, // dim->scale_x.val(),
          .scale_y = 1.0, // dim->scale_y.val(),
        };
        
        auto &fragment = compositing_node->graphics;
        fragment.clear();
        event_handler_->draw_fragment(fragment);
        if (!fragment.empty()) {
          for (const auto &elem: fragment.elements) {
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
      
      component_base_t* find_by_coords(int x, int y) override {
        component_base_t* found = nullptr;
        for (auto c = children.rbegin(); c != children.rend(); ++c) {
          found = (*c)->find_by_coords(x, y);
          if (nullptr != found) {
            return found;
          }
        }
        if (cx.val() <= x && x < cx.val() + cw.val()) {
          if (cy.val() <= y && y < cy.val() + ch.val()) {
            return this;
          }
        }
        return nullptr;
      }
      
      component_dimensional_relations_t get_dimensional_relations() override {
        return {
          this->_x,
          this->_y,
          this->_w,
          this->_h,
          this->_w_has_changed,
          this->_h_has_changed,
          this->cx,
          this->cy,
          this->cw,
          this->ch,
          this->_margin_top,
          this->_margin_bottom,
          this->_margin_left,
          this->_margin_right,
          this->_padding_top,
          this->_padding_bottom,
          this->_padding_left,
          this->_padding_right,
        };
      }
    };
}

//#include "../graphics/vg.h"
//
//struct CanvasState: public component_state_t {
//
//};
//
//struct CanvasEventHandler: public event_handler_t {
//  CanvasState* state;
//  int* props;
//
//  std::vector<component_holder_t> on_redraw() final {
//    return {};
//  }
//};
//
//struct Canvas
//  : public component_t<CanvasEventHandler, Canvas> {
//  CYDUI_COMPONENT_METADATA(Canvas)
//  using state_t = CanvasState;
//  using event_handler_t = CanvasEventHandler;
//  struct props_t {
//
//  } props {};
//
//  Canvas() = default;
//  explicit Canvas(props_t props): props(props) { }
//
//  Canvas &operator()(const std::function<vg::vg_fragment_t()> &render) {
//    auto frag = render();
//    return *this;
//  }
//};
//
//auto test(auto a, auto b) {
//  return a + b;
//}
//
//void sadfasdfasddf() {
//  auto i = test(2, 4.0);
//  auto j = test("asd", 'a');
//}

//struct $ {
//  $(int a, float f) {
//
//  };
//};
//
//constexpr long operator "" _px(unsigned long long val) {
//  return val;
//}
//
//using namespace std::chrono_literals;
//void tasdfest() {
//  $ {2, 1.4f};
//  1ms;
//  10_px;
//}

#endif //CYD_UI_COMPONENT_H