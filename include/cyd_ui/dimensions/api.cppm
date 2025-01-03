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

 template <typename S>
 const S& get_value(dimension<S>& dimension) {
  return dimension.impl()->value_;
 }

 template <typename S>
 const S& get_value(const dimension<S>& dimension) {
  return dimension.impl()->value_;
 }
}