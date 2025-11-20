/*! \file  function.cppm
 *! \brief
 *!
 */

export module cydui.dimensions:function;

import std;
import fabric.logging;

export import :types;

export namespace cydui::dimensions {
  template <typename Type>
  struct function: std::function<Type()> {
    using depset_type = std::unordered_set<std::shared_ptr<dimension_impl<Type>>>;
    depset_type       dependencies {};
    std::string name {};

    function(
      auto&&      fn,
      depset_type dependencies,
      std::string name = "")
        : std::function<Type()>(fn),
          dependencies(dependencies),
          name(name) {}
  };
} // namespace cydui::dimensions
