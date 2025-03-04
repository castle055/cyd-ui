// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>
#include <tracy/Tracy.hpp>

export module cydui.graphics.vector.elements.group;

import std;

export import cydui.dimensions;
export import cydui.graphics.vector.element;

export namespace vg {
  template<typename... T>
  struct accepts_content {
    accepts_content &with(std::vector<std::variant<T...>> _content_) {
      this->content = _content_;
      return *this;
    }

    std::vector<std::variant<T...>> content{};
  };

  struct group:
    element_t,
    attr_cx<group>,
    attrs_core<group>,
    attrs_fill<group>,
    attrs_stroke<group>,
    accepts_content<group> {
    group() {
      //TracyAllocN(this, sizeof(decltype(*this)), "fragment_elements");
    }
    ~group() override {
      //TracyFreeN(this, "fragment_elements");
    }
    void apply_to(pixelmap_editor_t &editor) const override {
      apply_stroke(editor);
    }

    footprint get_footprint() const override {
      return {};
    }
  };
}
