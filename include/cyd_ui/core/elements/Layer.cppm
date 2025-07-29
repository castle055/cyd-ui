// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.layer;

import std;
import reflect;

export import cydui.elements;
export import cydui.transform;


namespace cydui {
  export enum class overflow_e { GROW, HIDE, SCROLL };
  export enum class position_e { ABSOLUTE, RELATIVE };

  export struct LayerViewport {
    vec2 position;
    vec2 origin;
    vec2 size;
  };

  export struct LayerStyle {
    elements::Rectangle background;
    bool animated{false};
    enum {
      OVERLAY,
    } op = OVERLAY;
    overflow_e x_overflow = overflow_e::HIDE;
    overflow_e y_overflow = overflow_e::HIDE;
    position_e position = position_e::RELATIVE;
  };

  export struct Layer {
    ElementVector            elements {};
    AffineTransform          transform {};
    LayerViewport            viewport {};
    vec2                     size {};
    LayerStyle               style {};
    std::optional<refl::any> render_data {std::nullopt};
    bool                     dirty {true};

    void mark_dirty() {
      dirty = true;
    }
  };
} // namespace cydui
