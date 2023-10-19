//
// Created by castle on 10/16/23.
//

#ifndef CYD_UI_VG_ATTRIBUTES_H
#define CYD_UI_VG_ATTRIBUTES_H

#include "color.h"
#include "pixelmap_editor.h"

namespace vg {
    struct paint_t {
      enum {
        COLOR,
      } type = COLOR;
      union {
        color::Color color = "#000000"_color;
        //color::Color coaor = "#FBAD1E"_color;
        //color::Color coror = "#FCAE1E"_color;
      };
    };

#define VG_ATTRIBUTE(TYPE, NAME, DEFAULT) \
    template<typename E>                  \
    struct attr_##NAME {                  \
      inline E &NAME(TYPE& _##NAME##_) {  \
        this->_##NAME = _##NAME##_;       \
        return *(E*)this;                 \
      }                                   \
      inline E &NAME(TYPE&& _##NAME##_) { \
        this->_##NAME = _##NAME##_;       \
        return *(E*)this;                 \
      }                                   \
      TYPE _##NAME = DEFAULT;             \
    }
    
    // * id
    VG_ATTRIBUTE(std::string, id, "");
    // * class
    VG_ATTRIBUTE(std::vector<std::string>, style_class, std::vector<std::string> {});
    
    // * x  - x-axis coordinate
    VG_ATTRIBUTE(int, x, 0);
    // * y  - y-axis coordinate
    VG_ATTRIBUTE(int, y, 0);
    // * r  - radius
    VG_ATTRIBUTE(int, r, 0);
    // * rx - radius in x-axis
    VG_ATTRIBUTE(int, rx, 0);
    // * ry - radius in y-axis
    VG_ATTRIBUTE(int, ry, 0);
    // * x1 - 1st point x-axis coordinate
    VG_ATTRIBUTE(int, x1, 0);
    // * y1 - 1st point y-axis coordinate
    VG_ATTRIBUTE(int, y1, 0);
    // * x2 - 2nd point x-axis coordinate
    VG_ATTRIBUTE(int, x2, 0);
    // * y2 - 2nd point y-axis coordinate
    VG_ATTRIBUTE(int, y2, 0);
    // * cx - center x-axis coordinate
    VG_ATTRIBUTE(int, cx, 0);
    // * cy - center y-axis coordinate
    VG_ATTRIBUTE(int, cy, 0);
    // * points
    using point_list_t = std::vector<std::array<int, 2>>;
    
    VG_ATTRIBUTE(point_list_t, points, point_list_t {});
    
    // * d - path str
    VG_ATTRIBUTE(std::string, path_str, std::string {});
    
    // * width
    VG_ATTRIBUTE(int, width, 0);
    // * height
    VG_ATTRIBUTE(int, height, 0);
    
    // * hidden - replaces display="none" from SVG
    VG_ATTRIBUTE(bool, hidden, 1.0);
    // * opacity
    VG_ATTRIBUTE(float, opacity, 1.0);
    
    // ?* STROKE ATTRIBUTES
    // * stroke
    VG_ATTRIBUTE(paint_t, stroke, paint_t {});
    // * stroke-dasharray
    VG_ATTRIBUTE(std::vector<double>, stroke_dasharray, std::vector<double> {});
    // * stroke-dashoffset
    VG_ATTRIBUTE(double, stroke_dashoffset, 0.0);
    
    // * stroke-linecap
    //enum class stroke_linecap_e {
    //  BUTT,
    //  ROUND,
    //  SQUARE,
    //};
    using stroke_linecap_e = Cairo::Context::LineCap;
    
    VG_ATTRIBUTE(stroke_linecap_e, stroke_linecap, stroke_linecap_e::BUTT);
    
    // * stroke-linejoin
    //enum class stroke_linejoin_e {
    //  ARCS,
    //  BEVEL,
    //  MITER,
    //  MITER_CLIP,
    //  ROUND,
    //};
    using stroke_linejoin_e = Cairo::Context::LineJoin;
    
    VG_ATTRIBUTE(stroke_linejoin_e, stroke_linejoin, stroke_linejoin_e::MITER);
    
    // * stroke-miterlimit
    VG_ATTRIBUTE(int, stroke_miterlimit, 4);
    // * stroke-opacity
    VG_ATTRIBUTE(float, stroke_opacity, 1.0);
    // * stroke-width
    VG_ATTRIBUTE(int, stroke_width, 0);
    
    // ?* TEXT ATTRIBUTES
    // * font-family
    VG_ATTRIBUTE(std::string, font_family, " ");
    // * font-size
    VG_ATTRIBUTE(int, font_size, 12);
    //// * font-size-adjust
    // !VG_ATTRIBUTE(double, font_size_adjust, 0.0); What is this?
    // * font-stretch
    VG_ATTRIBUTE(double, font_stretch, 1.0);
    
    // * font-style
    enum class font_style_e {
      NORMAL,
      ITALIC,
      OBLIQUE,
    };
    
    VG_ATTRIBUTE(font_style_e, font_style, font_style_e::NORMAL);
    
    // * font-weight
    enum class font_weight_e {
      NORMAL,
      BOLD,
    };
    
    VG_ATTRIBUTE(font_weight_e, font_weight, font_weight_e::NORMAL);
    
    // * text-anchor
    enum class text_anchor_e {
      START,
      MIDDLE,
      END,
    };
    
    VG_ATTRIBUTE(text_anchor_e, text_anchor, text_anchor_e::START);
    
    // * text-decoration
    struct text_decoration_t {
      bool underline = false;
      bool overline = false;
      bool strike_through = false;
    };
    
    VG_ATTRIBUTE(text_decoration_t, text_decoration, text_decoration_t {});
    
    // ?* COLOR ATTRIBUTES
    // * color
    // * fill
    VG_ATTRIBUTE(paint_t, fill, paint_t {});
    // * fill-opacity
    VG_ATTRIBUTE(double, fill_opacity, 1.0);
    
    // * fill-rule
    using fill_rule_e = Cairo::Context::FillRule;
    
    VG_ATTRIBUTE(fill_rule_e, fill_rule, fill_rule_e::WINDING);
    
    // ?* CLIP ATTRIBUTES
    // * clip-path
    // * clip-rule
    // * clipPathUnits
    
    // ? ATTRIBUTE GROUPS
    template<typename T>
    struct attrs_stroke:
      attr_stroke<T>,
      attr_stroke_width<T>,
      attr_stroke_opacity<T>,
      attr_stroke_miterlimit<T>,
      attr_stroke_dasharray<T>,
      attr_stroke_dashoffset<T>,
      attr_stroke_linejoin<T>,
      attr_stroke_linecap<T> {
    protected:
      void apply_stroke(pixelmap_editor_t &editor) const {
        editor->set_line_width(this->_stroke_width);
        editor->set_line_cap(this->_stroke_linecap);
        editor->set_line_join(this->_stroke_linejoin);
        editor->set_dash(this->_stroke_dasharray, this->_stroke_dashoffset);
        editor->set_miter_limit(this->_stroke_miterlimit);
      }
    };
    
    template<typename T>
    struct attrs_fill:
      attr_fill<T>,
      attr_fill_opacity<T>,
      attr_fill_rule<T> {
    protected:
      void apply_fill(pixelmap_editor_t &editor) const {
        editor->set_fill_rule(this->_fill_rule);
      }
    };
    
    template<typename T>
    struct attrs_core:
      attr_id<T>,
      attr_style_class<T>,
      attr_hidden<T>,
      attr_opacity<T> {
    };
}

#endif //CYD_UI_VG_ATTRIBUTES_H


// * preserveAspectRatio

// * overflow
// * spacing
// * style
// * transform
// * transform-origin

// ? --------------------------------------------------------------------------
// * color-interpolation
// * color-interpolation-filters
// * color-profile

// * font-variant
// * string
// * strikethrough-position
// * strikethrough-thickness
// * text-rendering
// * z
// * rotate
// * radius
// * origin
// * scale
// * orientation (DEPRECATED IN SVG)
// * flood-color
// * flood-opacity
// * clip (DEPRECATED)

// * accent-height
// * accumulate
// * additive
// * alignment-baseline
// * alphabetic
// * amplitude
// * arabic-form
// * ascent
// * attributeName
// * attributeType
// * azimuth
// * baseFrequency
// * baseline-shift
// * baseProfile
// * bbox
// * begin
// * bias
// * by
// * calcMode
// * cap-height
// * class
// * contentScriptType
// * contentStyleType
// * cursor
// * data-*
// * decoding
// * descent
// * diffuseConstant
// * direction
// * display
// * divisor
// * dominant-baseline
// * dur
// * dx
// * dy
// * edgeMode
// * elevation
// * enable-background
// * end
// * exponent
// * filter
// * filterRes
// * filterUnits
// * fr
// * from
// * fx
// * fy
// * g1
// * g2
// * glyph-name
// * glyph-orientation-horizontal
// * glyph-orientation-vertical
// * gradientTransform
// * gradientUnits
// * hanging
// * horiz-adv-x
// * horiz-origin-x
// * horiz-origin-y
// * href
// * id
// * ideographic
// * image-rendering
// * in
// * in2
// * intercept
// * k
// * k1
// * k2
// * k3
// * k4
// * kernelMatrix
// * kernelUnitLength
// * kerning
// * keyPoints
// * keySplines
// * keyTimes
// * lang
// * lengthAdjust
// * letter-spacing
// * lighting-color
// * limitingConeAngle
// * marker-end
// * marker-mid
// * marker-start
// * markerHeight
// * markerUnits
// * markerWidth
// * mask
// * maskContentUnits
// * maskUnits
// * mathematical
// * max
// * media
// * method
// * Experimental
// * min
// * mode
// * name
// * numOctaves
// * onclick
// * order
// * orient
// * operator
// * overline-position
// * overline-thickness
// * paint-order
// * panose-1
// * path
// * pathLength
// * patternContentUnits
// * patternTransform
// * patternUnits
// * pointer-events
// * pointsAtX
// * pointsAtY
// * pointsAtZ
// * preserveAlpha
// * primitiveUnits
// * refX
// * refY
// * repeatCount
// * repeatDur
// * requiredFeatures
// * restart
// * result
// * Experimental
// * seed
// * shape-rendering
// * side
// * Experimental
// * slope
// * specularConstant
// * specularExponent
// * spreadMethod
// * startOffset
// * stdDeviation
// * stemh
// * stemv
// * stitchTiles
// * stop-color
// * stop-opacity
// * surfaceScale
// * SVG attribute: crossorigin
// * SVG Conditional Processing Attributes
// * SVG Core Attributes
// * SVG Event Attributes
// * SVG Presentation Attributes
// * SVG Styling Attributes
// * systemLanguage
// * tabindex
// * tableValues
// * target
// * targetX
// * targetY
// * textLength
// * to
// * type
// * u1
// * u2
// * underline-position
// * underline-thickness
// * unicode
// * unicode-bidi
// * unicode-range
// * units-per-em
// * v-alphabetic
// * v-hanging
// * v-ideographic
// * v-mathematical
// * values
// * vector-effect
// * version
// * vert-adv-y
// * vert-origin-x
// * vert-origin-y
// * viewBox
// * viewTarget
// * visibility
// * widths
// * word-spacing
// * writing-mode
// * x-height
// * xChannelSelector
// * xlink:arcrole
// * xlink:href
// * xlink:show
// * xlink:title
// * xlink:type
// * xml:base
// * xml:lang
// * xml:space
// * yChannelSelector
// * zoomAndPan

