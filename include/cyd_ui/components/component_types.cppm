// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.components:type;

import std;

export import :dimensions;

export {
namespace cyd::ui::components {
    
    struct component_state_t;
    using component_state_ref = std::shared_ptr<component_state_t>;
    struct component_base_t;
    struct event_handler_t;
    
    template<typename EVH>
    concept ComponentEventHandlerConcept = requires(EVH evh) {
      std::is_base_of_v<event_handler_t, EVH>;
    };
    
    template<ComponentEventHandlerConcept EVH, typename T>
    struct component_t;
    
    template<typename C>
    concept ComponentConcept = requires(C c) {
      requires std::derived_from<std::remove_reference_t<C>, component_t<typename C::event_handler_t, C>>;
      typename C::props_t;
      typename C::state_t;
      typename C::event_handler_t;
      {C::NAME} -> std::convertible_to<std::string>;
    };
    
    struct component_dimensional_relations_t {
      cyd::ui::dimensions::dimension_t &x;
      cyd::ui::dimensions::dimension_t &y;
      cyd::ui::dimensions::dimension_t &w;
      cyd::ui::dimensions::dimension_t &h;
      bool &fixed_w;
      bool &fixed_h;
      
      cyd::ui::dimensions::dimension_t &cx;
      cyd::ui::dimensions::dimension_t &cy;
      cyd::ui::dimensions::dimension_t &cw;
      cyd::ui::dimensions::dimension_t &ch;
      
      cyd::ui::dimensions::dimension_t &margin_top;
      cyd::ui::dimensions::dimension_t &margin_bottom;
      cyd::ui::dimensions::dimension_t &margin_left;
      cyd::ui::dimensions::dimension_t &margin_right;
      
      cyd::ui::dimensions::dimension_t &padding_top;
      cyd::ui::dimensions::dimension_t &padding_bottom;
      cyd::ui::dimensions::dimension_t &padding_left;
      cyd::ui::dimensions::dimension_t &padding_right;
    };
}

}
