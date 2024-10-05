/*! \file  api.cppm
 *! \brief 
 *!
 */

export module cydui.dimensions:api;

import std;

import fabric.logging;

export import :types;
export import :compute;
export import :dimension;


export namespace cyd::ui::dimensions {
 template <typename T>
 compute_result_t<T> compute(dimension<T>& dimension) {
  return compute_dimension(dimension);
 }

 template <typename T>
 const T& get_value(dimension<T>& dimension) {
  return dimension.impl()->value_;
 }
}