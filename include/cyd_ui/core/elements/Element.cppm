// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.elements;

import std;
import reflect;

export import cydui.elements.Line;
export import cydui.elements.Arc;
export import cydui.elements.Circle;
export import cydui.elements.Ellipse;
export import cydui.elements.Polygon;
export import cydui.elements.Polyline;
export import cydui.elements.Rectangle;
export import cydui.elements.Text;
export import cydui.elements.Pixelmap;


namespace cydui {
  export using Element = std::variant<
    elements::Line,
    elements::Arc,
    elements::Circle,
    elements::Ellipse,
    elements::Polygon,
    elements::Polyline,
    elements::Rectangle,
    elements::Pixelmap,
    elements::Text>;

  export using ElementVector = std::vector<Element>;
} // namespace cydui

// * <image>

// * <linearGradient>
// * <radialGradient>

// * <text>
// * <textPath>
// * <tspan>

// * <mask>
// * <clipPath>

// * <defs>

// * <filter>
// * <feBlend>
// * <feColorMatrix>
// * <feComponentTransfer>
// * <feComposite>
// * <feConvolveMatrix>
// * <feDiffuseLighting>
// * <feDisplacementMap>
// * <feDistantLight>
// * <feDropShadow>
// * <feFlood>
// * <feFuncA>
// * <feFuncB>
// * <feFuncG>
// * <feFuncR>
// * <feGaussianBlur>
// * <feImage>
// * <feMerge>
// * <feMergeNode>
// * <feMorphology>
// * <feOffset>
// * <fePointLight>
// * <feSpecularLighting>
// * <feSpotLight>
// * <feTile>
// * <feTurbulence>

// * <animate>
// * <animateMotion>
// * <animateTransform>
// * <stop>

// * <pattern>

// * <svg>
// * <a>
// * <cursor>
// * <foreignObject>
// * <glyph>
// * <glyphRef>
// * <marker>
// * <metadata>
// * <missing-glyph>
// * <mpath>
// * <script>
// * <set>
// * <style>
// * <switch>
// * <symbol>
// * <title> — the SVG accessible name element
// * <desc>
// * <tref>
// * <use>
// * <view>
// * <hkern>
// * <vkern>

// * <font>
// * <font-face-format>
// * <font-face-name>
// * <font-face-src>
// * <font-face-uri>
// * <font-face>
