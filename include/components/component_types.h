// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_COMPONENT_TYPES_H
#define CYD_UI_COMPONENT_TYPES_H

#include <type_traits>
#include <concepts>
#include <string>

#include "dimensions.hpp"

namespace cyd::ui::components {
    
    struct component_state_t;
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
      requires std::derived_from<C, component_t<typename C::event_handler_t, C>>;
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

#endif //CYD_UI_COMPONENT_TYPES_H
