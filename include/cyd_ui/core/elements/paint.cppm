// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.paint;

import std;
import reflect;
export import quantify;

export import cydui.graphics.types;

export namespace cydui {
  namespace paints {
    struct Solid {
      Color color {};

      Solid()
          : color("#00000000"_color) {}
      Solid(Color color)
          : color(color) {}

      bool operator==(const Solid& rhl) const {
        return color == rhl.color;
      }
    };

    struct LinearGradient {
      struct ColorStop {
        double offset {};
        Color  color {};

        bool operator==(const ColorStop& rhl) const {
          return offset == rhl.offset && color == rhl.color;
        }
      };

      std::vector<ColorStop> color_stops {};
      int                    x0 {}, y0 {};
      int                    x1 {}, y1 {};

      bool operator==(const LinearGradient& rhl) const {
        return x0 == rhl.x0 && //
               x1 == rhl.x1 && //
               y0 == rhl.y0 && //
               y1 == rhl.y1 && //
               color_stops == rhl.color_stops;
      }
    };

    struct RadialGradient: LinearGradient {
      int r0 {}, r1 {};

      bool operator==(const RadialGradient& rhl) const {
        return x0 == rhl.x0 && //
               x1 == rhl.x1 && //
               y0 == rhl.y0 && //
               y1 == rhl.y1 && //
               r0 == rhl.r0 && //
               r1 == rhl.r1 && //
               color_stops == rhl.color_stops;
      }
    };
  } // namespace paints

  using Paint = std::variant<paints::Solid, paints::LinearGradient, paints::RadialGradient>;
} // namespace cydui
