//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_TYPES_H
#define CYD_UI_COMPONENT_TYPES_H

#include <type_traits>
#include <concepts>
#include <string>

#include "dimensions.hpp"

namespace cydui::components {
    
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
      cydui::dimensions::dimension_t &x;
      cydui::dimensions::dimension_t &y;
      cydui::dimensions::dimension_t &w;
      cydui::dimensions::dimension_t &h;
      bool &fixed_w;
      bool &fixed_h;
      
      cydui::dimensions::dimension_t &cx;
      cydui::dimensions::dimension_t &cy;
      cydui::dimensions::dimension_t &cw;
      cydui::dimensions::dimension_t &ch;
      
      cydui::dimensions::dimension_t &margin_top;
      cydui::dimensions::dimension_t &margin_bottom;
      cydui::dimensions::dimension_t &margin_left;
      cydui::dimensions::dimension_t &margin_right;
      
      cydui::dimensions::dimension_t &padding_top;
      cydui::dimensions::dimension_t &padding_bottom;
      cydui::dimensions::dimension_t &padding_left;
      cydui::dimensions::dimension_t &padding_right;
    };
}

#endif //CYD_UI_COMPONENT_TYPES_H
