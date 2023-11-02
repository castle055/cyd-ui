//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_COMPONENT_TYPES_H
#define CYD_UI_COMPONENT_TYPES_H

#include <type_traits>
#include <concepts>
#include <string>

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
  std::derived_from<C, component_t<typename C::event_handler_t, C>>;
  typename C::props_t;
  typename C::state_t;
  typename C::event_handler_t;
  {C::NAME} -> std::convertible_to<std::string>;
};

#endif //CYD_UI_COMPONENT_TYPES_H
