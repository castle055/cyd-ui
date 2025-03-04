// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.fragment;

import std;

import fabric.memory.multidim_data;
import fabric.type_aliases;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  struct fragment_t {
    std::vector<std::shared_ptr<element_t>> elements{};

    void clear() {
      elements.clear();
    }

    bool empty() const {
      return elements.empty();
    }

    template<typename... T>
    void append(T &&... _elements) {
      static_assert(
        (std::derived_from<std::remove_reference_t<T>, element_t> && ...),
        "Elements must derive from vg_element_t."
      );
      (elements.push_back(make_element<std::remove_reference_t<T>>(std::forward<std::remove_reference_t<T>>(_elements))), ...);
    }

    template<typename T, typename... Args>
    T& draw(Args&&... args) {
      static_assert(
        std::derived_from<std::remove_reference_t<T>, element_t>,
        "Elements must derive from vg_element_t."
      );
      elements.emplace_back(new std::remove_reference_t<T>{std::forward<Args &&>(args)...});
      return *static_cast<T*>(elements.back().get());
    }

  private:
    template<typename T>
    static std::shared_ptr<element_t> make_element(T&& element) {
      T* ptr = new T{std::forward<T>(element)};
      return std::shared_ptr<element_t>(ptr);
    }
  };
}