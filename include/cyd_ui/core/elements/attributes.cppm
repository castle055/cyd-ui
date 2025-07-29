// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module cydui.elements.attributes;

import std;
export import quantify;

export import cydui.paint;

export namespace cydui::elements::attributes {
  struct BorderRadius {
    int rx, ry;

    BorderRadius(double r)
        : rx(r),
          ry(r) {}
    BorderRadius(
      double rx,
      double ry)
        : rx(rx),
          ry(ry) {}

    template <
      typename UNIT,
      typename T>
    BorderRadius(
      const quantify::quantity<
        UNIT,
        T>& rx,
      const quantify::quantity<
        UNIT,
        T>& ry)
        : rx(rx.value_as_base_unit()),
          ry(ry.value_as_base_unit()) {}

    template <
      typename UNIT,
      typename T>
    BorderRadius(
      const quantify::quantity<
        UNIT,
        T>& r)
        : rx(r.value_as_base_unit()),
          ry(r.value_as_base_unit()) {}
  };

  /**
   * FillRule is used to select how paths are filled. For both fill rules,
   * whether or not a point is included in the fill is determined by taking a ray
   * from that point to infinity and looking at intersections with the path. The
   * ray can be in any direction, as long as it doesn't pass through the end
   * point of a segment or have a tricky intersection such as intersecting
   * tangent to the path. (Note that filling is not actually implemented in this
   * way. This is just a description of the rule that is applied.)
   *
   * The default fill rule is FillRule::WINDING.
   *
   * New entries may be added in future versions.
   **/
  enum class FillRule {
    /**
     * If the path crosses the ray from left-to-right, counts +1. If the path
     * crosses the ray from right to left, counts -1. (Left and right are
     * determined from the perspective of looking along the ray from the
     * starting point.) If the total count is non-zero, the point will be
     * filled.
     */
    WINDING,

    /**
     * Counts the total number of intersections, without regard to the
     * orientation of the contour. If the total number of intersections is odd,
     * the point will be filled.
     */
    EVEN_ODD
  };

  /**
   * Specifies how to render the endpoints of the path when stroking.
   *
   * The default line cap style is LineCap::BUTT.
   **/
  enum class LineCap {
    /**
     * Start(stop) the line exactly at the start(end) point
     */
    BUTT,

    /**
     * Use a round ending, the center of the circle is the end point
     */
    ROUND,

    /**
     * Use a squared ending, the center of the square is the end point
     */
    SQUARE
  };

  /**
   * Specifies how to render the junction of two lines when stroking.
   *
   * The default line join style is LineJoin::MITER.
   */
  enum class LineJoin {
    /**
     * Use a sharp (angled) corner, see Context::set_miter_limit()
     */
    MITER,

    /**
     * Use a rounded join, the center of the circle is the joint point
     */
    ROUND,

    /**
     * Use cut-off join, the join is cut off at half the line width from the
     * join point
     */
    BEVEL,
  };

  /**
   * Specifies variants of a font face based on their slant.
   */
  enum class FontStyle { NORMAL, ITALIC, OBLIQUE };

  /**
   * Specifies variants of a font face based on their weight.
   */
  enum class FontWeight {
    THIN   = 100,
    LIGHT  = 300,
    NORMAL = 400,
    MEDIUM = 500,
    BOLD   = 700,
    HEAVY  = 900,
  };

  enum class TextAlign {
    LEFT,
    CENTER,
    RIGHT,
  };

  enum class TextStretch {
    ULTRA_CONDENSED,
    EXTRA_CONDENSED,
    CONDENSED,
    SEMI_CONDENSED,
    NORMAL,
    SEMI_EXPANDED,
    EXPANDED,
    EXTRA_EXPANDED,
    ULTRA_EXPANDED
  };

#define ELEMENT_ATTRIBUTE_SETTER(TYPE, NAME)                                                       \
  inline E& NAME(const TYPE& _##NAME##_) {                                                         \
    this->_##NAME = _##NAME##_;                                                                    \
    return *(E*)this;                                                                              \
  }                                                                                                \
  inline E& NAME(TYPE&& _##NAME##_) {                                                              \
    this->_##NAME = _##NAME##_;                                                                    \
    return *(E*)this;                                                                              \
  }

#define ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME)                                                   \
  template <typename UNIT, typename T>                                                             \
  inline E& NAME(const quantify::quantity<UNIT, T>& _##NAME##_) {                                  \
    this->_##NAME = _##NAME##_.value_as_base_unit();                                               \
    return *(E*)this;                                                                              \
  }                                                                                                \
  template <typename UNIT, typename T>                                                             \
  inline E& NAME(quantify::quantity<UNIT, T>&& _##NAME##_) {                                       \
    this->_##NAME = _##NAME##_.value_as_base_unit();                                               \
    return *(E*)this;                                                                              \
  }                                                                                                \
  inline E& NAME(int _##NAME##_) {                                                                 \
    this->_##NAME = _##NAME##_;                                                                    \
    return *(E*)this;                                                                              \
  }

#define ELEMENT_ATTRIBUTE(TYPE, NAME, DEFAULT)                                                     \
  template <typename E>                                                                            \
  struct attr_##NAME {                                                                             \
    ELEMENT_ATTRIBUTE_SETTER(                                                                      \
      TYPE,                                                                                        \
      NAME)                                                                                        \
    TYPE _##NAME = DEFAULT;                                                                        \
  }

#define ELEMENT_ATTRIBUTE_DIMENSION(NAME, DEFAULT)                                                 \
  template <typename E>                                                                            \
  struct attr_##NAME {                                                                             \
    ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME)                                                       \
    int _##NAME = DEFAULT;                                                                         \
  }

#define ELEMENT_FOUR_ATTRIBUTES(TYPE, NAME, DEFAULT, SUBNAME1, SUBNAME2, SUBNAME3, SUBNAME4)       \
  template <typename E>                                                                            \
  struct attr_##NAME {                                                                             \
    ELEMENT_ATTRIBUTE_SETTER(                                                                      \
      TYPE,                                                                                        \
      NAME##_##SUBNAME1)                                                                           \
    ELEMENT_ATTRIBUTE_SETTER(                                                                      \
      TYPE,                                                                                        \
      NAME##_##SUBNAME2)                                                                           \
    ELEMENT_ATTRIBUTE_SETTER(                                                                      \
      TYPE,                                                                                        \
      NAME##_##SUBNAME3)                                                                           \
    ELEMENT_ATTRIBUTE_SETTER(                                                                      \
      TYPE,                                                                                        \
      NAME##_##SUBNAME4)                                                                           \
    inline E& NAME(                                                                                \
      const TYPE& _##SUBNAME1##_,                                                                  \
      const TYPE& _##SUBNAME2##_,                                                                  \
      const TYPE& _##SUBNAME3##_,                                                                  \
      const TYPE& _##SUBNAME4##_) {                                                                \
      this->NAME##_##SUBNAME1(_##SUBNAME1##_);                                                     \
      this->NAME##_##SUBNAME2(_##SUBNAME2##_);                                                     \
      this->NAME##_##SUBNAME3(_##SUBNAME3##_);                                                     \
      this->NAME##_##SUBNAME4(_##SUBNAME4##_);                                                     \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(const TYPE& _##NAME##_) {                                                       \
      this->NAME##_##SUBNAME1(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME2(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME3(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME4(_##NAME##_);                                                         \
      return *(E*)this;                                                                            \
    }                                                                                              \
    TYPE _##NAME##_##SUBNAME1 = DEFAULT;                                                           \
    TYPE _##NAME##_##SUBNAME2 = DEFAULT;                                                           \
    TYPE _##NAME##_##SUBNAME3 = DEFAULT;                                                           \
    TYPE _##NAME##_##SUBNAME4 = DEFAULT;                                                           \
  }

#define ELEMENT_FOUR_DIMENSION_ATTRIBUTES(NAME, DEFAULT, SUBNAME1, SUBNAME2, SUBNAME3, SUBNAME4)   \
  template <typename E>                                                                            \
  struct attr_##NAME {                                                                             \
    ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME##_##SUBNAME1)                                          \
    ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME##_##SUBNAME2)                                          \
    ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME##_##SUBNAME3)                                          \
    ELEMENT_ATTRIBUTE_DIMENSION_SETTER(NAME##_##SUBNAME4)                                          \
    template <                                                                                     \
      typename UNIT,                                                                               \
      typename T>                                                                                  \
    inline E& NAME(                                                                                \
      const quantify::quantity<                                                                    \
        UNIT,                                                                                      \
        T>& _##SUBNAME1##_,                                                                        \
      const quantify::quantity<                                                                    \
        UNIT,                                                                                      \
        T>& _##SUBNAME2##_,                                                                        \
      const quantify::quantity<                                                                    \
        UNIT,                                                                                      \
        T>& _##SUBNAME3##_,                                                                        \
      const quantify::quantity<                                                                    \
        UNIT,                                                                                      \
        T>& _##SUBNAME4##_) {                                                                      \
      this->NAME##_##SUBNAME1(_##SUBNAME1##_);                                                     \
      this->NAME##_##SUBNAME2(_##SUBNAME2##_);                                                     \
      this->NAME##_##SUBNAME3(_##SUBNAME3##_);                                                     \
      this->NAME##_##SUBNAME4(_##SUBNAME4##_);                                                     \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <                                                                                     \
      typename UNIT,                                                                               \
      typename T>                                                                                  \
    inline E& NAME(                                                                                \
      const quantify::quantity<                                                                    \
        UNIT,                                                                                      \
        T>& _##NAME##_) {                                                                          \
      this->NAME##_##SUBNAME1(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME2(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME3(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME4(_##NAME##_);                                                         \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(                                                                                \
      int _##SUBNAME1##_,                                                                          \
      int _##SUBNAME2##_,                                                                          \
      int _##SUBNAME3##_,                                                                          \
      int _##SUBNAME4##_) {                                                                        \
      this->NAME##_##SUBNAME1(_##SUBNAME1##_);                                                     \
      this->NAME##_##SUBNAME2(_##SUBNAME2##_);                                                     \
      this->NAME##_##SUBNAME3(_##SUBNAME3##_);                                                     \
      this->NAME##_##SUBNAME4(_##SUBNAME4##_);                                                     \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(int _##NAME##_) {                                                               \
      this->NAME##_##SUBNAME1(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME2(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME3(_##NAME##_);                                                         \
      this->NAME##_##SUBNAME4(_##NAME##_);                                                         \
      return *(E*)this;                                                                            \
    }                                                                                              \
    int _##NAME##_##SUBNAME1 = DEFAULT;                                                            \
    int _##NAME##_##SUBNAME2 = DEFAULT;                                                            \
    int _##NAME##_##SUBNAME3 = DEFAULT;                                                            \
    int _##NAME##_##SUBNAME4 = DEFAULT;                                                            \
  }


#define ELEMENT_FOUR_SIDED_ATTRIBUTE(TYPE, NAME, DEFAULT)                                          \
  ELEMENT_FOUR_ATTRIBUTES(TYPE, NAME, DEFAULT, top, right, bottom, left)

#define ELEMENT_FOUR_CORNER_ATTRIBUTE(TYPE, NAME, DEFAULT)                                         \
  ELEMENT_FOUR_ATTRIBUTES(TYPE, NAME, DEFAULT, top_left, top_right, bottom_right, bottom_left)

#define ELEMENT_FOUR_SIDED_DIMENSION_ATTRIBUTE(NAME, DEFAULT)                                      \
  ELEMENT_FOUR_DIMENSION_ATTRIBUTES(NAME, DEFAULT, top, right, bottom, left)

#define ELEMENT_FOUR_CORNER_DIMENSION_ATTRIBUTE(NAME, DEFAULT)                                     \
  ELEMENT_FOUR_DIMENSION_ATTRIBUTES(NAME, DEFAULT, top_left, top_right, bottom_right, bottom_left)

  //! @brief id
  ELEMENT_ATTRIBUTE(
    std::string,
    id,
    " ");
  //! @brief class
  ELEMENT_ATTRIBUTE(
    std::vector<std::string>,
    style_class,
    std::vector<std::string> {});

  //! @brief x  - x-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    x,
    0);
  //! @brief y  - y-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    y,
    0);
  //! @brief r  - radius
  ELEMENT_ATTRIBUTE_DIMENSION(
    r,
    0);
  //! @brief rx - radius in x-axis
  ELEMENT_ATTRIBUTE_DIMENSION(
    rx,
    0);
  //! @brief ry - radius in y-axis
  ELEMENT_ATTRIBUTE_DIMENSION(
    ry,
    0);
  //! @brief x1 - 1st point x-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    x1,
    0);
  //! @brief y1 - 1st point y-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    y1,
    0);
  //! @brief x2 - 2nd point x-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    x2,
    0);
  //! @brief y2 - 2nd point y-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    y2,
    0);
  //! @brief cx - center x-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    cx,
    0);
  //! @brief cy - center y-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    cy,
    0);
  //! @brief rotate - angle of rotation in degrees
  ELEMENT_ATTRIBUTE(
    double,
    rotate,
    0);
  //! @brief pivot_x - rotate pivot x-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    pivot_x,
    0);
  //! @brief pivot_y - rotate pivot y-axis coordinate
  ELEMENT_ATTRIBUTE_DIMENSION(
    pivot_y,
    0);
  //! @brief a1 - 1st angle in arcs in degrees
  ELEMENT_ATTRIBUTE(
    double,
    a1,
    0);
  //! @brief a1 - 2st angle in arcs in degrees
  ELEMENT_ATTRIBUTE(
    double,
    a2,
    0);
  ELEMENT_ATTRIBUTE(
    bool,
    include_center_point,
    false);

  //! @brief rotate - angle of rotation in degrees
  ELEMENT_ATTRIBUTE(
    double,
    scale_x,
    1);
  //! @brief rotate - angle of rotation in degrees
  ELEMENT_ATTRIBUTE(
    double,
    scale_y,
    1);

  using point_list_t = std::vector<std::array<int, 2>>;
  //! @brief points
  ELEMENT_ATTRIBUTE(
    point_list_t,
    points,
    point_list_t {});

  //! @brief d - path str
  ELEMENT_ATTRIBUTE(
    std::string,
    path_str,
    std::string {});

  //! @brief width
  ELEMENT_ATTRIBUTE_DIMENSION(
    w,
    0);
  //! @brief height
  ELEMENT_ATTRIBUTE_DIMENSION(
    h,
    0);

  //! @brief hidden - replaces display="none" from SVG
  ELEMENT_ATTRIBUTE(
    bool,
    hidden,
    1.0);
  //! @brief opacity
  ELEMENT_ATTRIBUTE(
    float,
    opacity,
    1.0);

  // ?* STROKE ATTRIBUTES
  //! @brief stroke
  ELEMENT_ATTRIBUTE(
    cydui::Paint,
    stroke,
    cydui::paints::Solid {"#00000000"_color});
  //! @brief stroke-dasharray
  ELEMENT_ATTRIBUTE(
    std::valarray<double>,
    stroke_dasharray,
    std::valarray<double> {});
  //! @brief stroke-dashoffset
  ELEMENT_ATTRIBUTE(
    double,
    stroke_dashoffset,
    0.0);

  //! @brief stroke-linecap
  ELEMENT_ATTRIBUTE(
    LineCap,
    stroke_linecap,
    LineCap::BUTT);

  //! @brief stroke-linejoin
  ELEMENT_ATTRIBUTE(
    LineJoin,
    stroke_linejoin,
    LineJoin::MITER);

  //! @brief stroke-miterlimit
  ELEMENT_ATTRIBUTE(
    int,
    stroke_miterlimit,
    4);
  //! @brief stroke-opacity
  ELEMENT_ATTRIBUTE(
    float,
    stroke_opacity,
    1.0);
  //! @brief stroke-width
  ELEMENT_ATTRIBUTE_DIMENSION(
    stroke_width,
    0);

  // ?* BORDER ATTRIBUTES
  //! @brief border
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    cydui::Paint,
    border,
    cydui::paints::Solid {});
  //! @brief border-dasharray
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    std::valarray<double>,
    border_dasharray,
    std::valarray<double> {});
  //! @brief border-dashoffset
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    double,
    border_dashoffset,
    0.0);

  //! @brief stroke-linecap
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    LineCap,
    border_linecap,
    LineCap::BUTT);

  //! @brief border-linejoin
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    LineJoin,
    border_linejoin,
    LineJoin::MITER);

  //! @brief border-miterlimit
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    int,
    border_miterlimit,
    4);
  //! @brief border-opacity
  ELEMENT_FOUR_SIDED_ATTRIBUTE(
    float,
    border_opacity,
    1.0);
  //! @brief border-radius
  ELEMENT_FOUR_CORNER_ATTRIBUTE(
    BorderRadius,
    border_radius,
    BorderRadius {0.0});
  //! @brief border-width
  ELEMENT_FOUR_SIDED_DIMENSION_ATTRIBUTE(
    border_width,
    0);

  // ?* TEXT ATTRIBUTES
  //! @brief font-family
  ELEMENT_ATTRIBUTE(
    std::string,
    font_family,
    "default");
  //! @brief font-size
  ELEMENT_ATTRIBUTE(
    int,
    font_size,
    12);
  // brief font-size-adjust
  // ELEMENT_ATTRIBUTE(double, font_size_adjust, 0.0); What is this?
  //! @brief font-stretch
  ELEMENT_ATTRIBUTE(
    TextStretch,
    font_stretch,
    TextStretch::NORMAL);

  //! @brief font-style
  ELEMENT_ATTRIBUTE(
    FontStyle,
    font_style,
    FontStyle::NORMAL);

  //! @brief font-weight
  ELEMENT_ATTRIBUTE(
    FontWeight,
    font_weight,
    FontWeight::NORMAL);

  //! @brief align
  ELEMENT_ATTRIBUTE(
    TextAlign,
    align,
    TextAlign::LEFT);

  //! @brief justify
  ELEMENT_ATTRIBUTE(
    bool,
    justify,
    false);

  struct text_decoration_t {
    bool underline      = false;
    bool overline       = false;
    bool strike_through = false;
  };

  //! @brief text-decoration
  ELEMENT_ATTRIBUTE(
    text_decoration_t,
    text_decoration,
    text_decoration_t {});

  // ?* COLOR ATTRIBUTES
  // brief color
  //! @brief fill
  ELEMENT_ATTRIBUTE(
    cydui::Paint,
    fill,
    cydui::paints::Solid {"#00000000"_color});

  //! @brief fill-opacity
  ELEMENT_ATTRIBUTE(
    double,
    fill_opacity,
    1.0);

  //! @brief fill-rule
  ELEMENT_ATTRIBUTE(
    FillRule,
    fill_rule,
    FillRule::WINDING);

  // ?* CLIP ATTRIBUTES
  // brief clip-path
  // brief clip-rule
  // brief clipPathUnits

  // ? ATTRIBUTE GROUPS
  template <typename T>
  struct attrs_stroke: attr_stroke<T>,
                       attr_stroke_width<T>,
                       attr_stroke_opacity<T>,
                       attr_stroke_miterlimit<T>,
                       attr_stroke_dasharray<T>,
                       attr_stroke_dashoffset<T>,
                       attr_stroke_linejoin<T>,
                       attr_stroke_linecap<T> {};

  template <typename T>
  struct attrs_border: attr_border<T>,
                       attr_border_width<T>,
                       attr_border_radius<T>,
                       attr_border_opacity<T>,
                       attr_border_miterlimit<T>,
                       attr_border_dasharray<T>,
                       attr_border_dashoffset<T>,
                       attr_border_linejoin<T>,
                       attr_border_linecap<T> {};

  template <typename T>
  struct attrs_fill: attr_fill<T>, attr_fill_opacity<T>, attr_fill_rule<T> {
  protected:
    // void apply_fill(cydui::graphics::pixelmap_editor_t& editor) const {
    //   editor->set_fill_rule(this->_fill_rule);
    // }

    // void set_source_to_fill(cydui::graphics::pixelmap_editor_t& editor) const {
    //   this->_fill.paint_data->apply_to_source(editor, this->_fill_opacity);
    // }
  };

  template <typename T>
  struct attrs_font: attr_font_family<T>,
                     attr_font_size<T>,
                     attr_font_stretch<T>,
                     attr_font_style<T>,
                     attr_font_weight<T> {};

  template <typename T>
  struct attrs_core:
      // attr_id<T>,
      // attr_style_class<T>,
      attr_hidden<T>,
      attr_opacity<T> {
    int  origin_x = 0;
    int  origin_y = 0;
    void _internal_set_origin(
      int x,
      int y) {
      origin_x = x;
      origin_y = y;
    }
  };
} // namespace cydui::elements::attributes


//! @brief preserveAspectRatio

//! @brief overflow
//! @brief spacing
//! @brief style
//! @brief transform
//! @brief transform-origin

// ? --------------------------------------------------------------------------
//! @brief color-interpolation
//! @brief color-interpolation-filters
//! @brief color-profile

//! @brief font-variant
//! @brief string
//! @brief strikethrough-position
//! @brief strikethrough-thickness
//! @brief text-rendering
//! @brief z
//! @brief rotate
//! @brief radius
//! @brief origin
//! @brief scale
//! @brief orientation (DEPRECATED IN SVG)
//! @brief flood-color
//! @brief flood-opacity
//! @brief clip (DEPRECATED)

//! @brief accent-height
//! @brief accumulate
//! @brief additive
//! @brief alignment-baseline
//! @brief alphabetic
//! @brief amplitude
//! @brief arabic-form
//! @brief ascent
//! @brief attributeName
//! @brief attributeType
//! @brief azimuth
//! @brief baseFrequency
//! @brief baseline-shift
//! @brief baseProfile
//! @brief bbox
//! @brief begin
//! @brief bias
//! @brief by
//! @brief calcMode
//! @brief cap-height
//! @brief class
//! @brief contentScriptType
//! @brief contentStyleType
//! @brief cursor
//! @brief data-*
//! @brief decoding
//! @brief descent
//! @brief diffuseConstant
//! @brief direction
//! @brief display
//! @brief divisor
//! @brief dominant-baseline
//! @brief dur
//! @brief dx
//! @brief dy
//! @brief edgeMode
//! @brief elevation
//! @brief enable-background
//! @brief end
//! @brief exponent
//! @brief filter
//! @brief filterRes
//! @brief filterUnits
//! @brief fr
//! @brief from
//! @brief fx
//! @brief fy
//! @brief g1
//! @brief g2
//! @brief glyph-name
//! @brief glyph-orientation-horizontal
//! @brief glyph-orientation-vertical
//! @brief gradientTransform
//! @brief gradientUnits
//! @brief hanging
//! @brief horiz-adv-x
//! @brief horiz-origin-x
//! @brief horiz-origin-y
//! @brief href
//! @brief id
//! @brief ideographic
//! @brief image-rendering
//! @brief in
//! @brief in2
//! @brief intercept
//! @brief k
//! @brief k1
//! @brief k2
//! @brief k3
//! @brief k4
//! @brief kernelMatrix
//! @brief kernelUnitLength
//! @brief kerning
//! @brief keyPoints
//! @brief keySplines
//! @brief keyTimes
//! @brief lang
//! @brief lengthAdjust
//! @brief letter-spacing
//! @brief lighting-color
//! @brief limitingConeAngle
//! @brief marker-end
//! @brief marker-mid
//! @brief marker-start
//! @brief markerHeight
//! @brief markerUnits
//! @brief markerWidth
//! @brief mask
//! @brief maskContentUnits
//! @brief maskUnits
//! @brief mathematical
//! @brief max
//! @brief media
//! @brief method
//! @brief Experimental
//! @brief min
//! @brief mode
//! @brief name
//! @brief numOctaves
//! @brief onclick
//! @brief order
//! @brief orient
//! @brief operator
//! @brief overline-position
//! @brief overline-thickness
//! @brief paint-order
//! @brief panose-1
//! @brief path
//! @brief pathLength
//! @brief patternContentUnits
//! @brief patternTransform
//! @brief patternUnits
//! @brief pointer-events
//! @brief pointsAtX
//! @brief pointsAtY
//! @brief pointsAtZ
//! @brief preserveAlpha
//! @brief primitiveUnits
//! @brief refX
//! @brief refY
//! @brief repeatCount
//! @brief repeatDur
//! @brief requiredFeatures
//! @brief restart
//! @brief result
//! @brief Experimental
//! @brief seed
//! @brief shape-rendering
//! @brief side
//! @brief Experimental
//! @brief slope
//! @brief specularConstant
//! @brief specularExponent
//! @brief spreadMethod
//! @brief startOffset
//! @brief stdDeviation
//! @brief stemh
//! @brief stemv
//! @brief stitchTiles
//! @brief stop-color
//! @brief stop-opacity
//! @brief surfaceScale
//! @brief SVG attribute: crossorigin
//! @brief SVG Conditional Processing Attributes
//! @brief SVG Core Attributes
//! @brief SVG Event Attributes
//! @brief SVG Presentation Attributes
//! @brief SVG Styling Attributes
//! @brief systemLanguage
//! @brief tabindex
//! @brief tableValues
//! @brief target
//! @brief targetX
//! @brief targetY
//! @brief textLength
//! @brief to
//! @brief type
//! @brief u1
//! @brief u2
//! @brief underline-position
//! @brief underline-thickness
//! @brief unicode
//! @brief unicode-bidi
//! @brief unicode-range
//! @brief units-per-em
//! @brief v-alphabetic
//! @brief v-hanging
//! @brief v-ideographic
//! @brief v-mathematical
//! @brief values
//! @brief vector-effect
//! @brief version
//! @brief vert-adv-y
//! @brief vert-origin-x
//! @brief vert-origin-y
//! @brief viewBox
//! @brief viewTarget
//! @brief visibility
//! @brief widths
//! @brief word-spacing
//! @brief writing-mode
//! @brief x-height
//! @brief xChannelSelector
//! @brief xlink:arcrole
//! @brief xlink:href
//! @brief xlink:show
//! @brief xlink:title
//! @brief xlink:type
//! @brief xml:base
//! @brief xml:lang
//! @brief xml:space
//! @brief yChannelSelector
//! @brief zoomAndPan
