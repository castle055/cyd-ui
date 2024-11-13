// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cairomm-1.16/cairomm/cairomm.h>

export module cydui.graphics:vg_attributes;

import std;

export import quantify;

export import :pixelmap_editor;

export
namespace vg {
  namespace paint {
    enum paint_type_e {
      SOLID,
      GRADIENT_LINEAR,
      GRADIENT_RADIAL,
    };

    struct base_i {
      virtual ~base_i() = default;
      virtual void         apply_to_source(pixelmap_editor_t& editor, double opacity) const = 0;
      virtual color::Color sample(int x, int y) const                                       = 0;

      virtual base_i* make_copy() const = 0;

      virtual paint_type_e get_type() const = 0;

      virtual bool operator==(const base_i& rhl) const = 0;
    };

    struct solid: public base_i {
      color::Color c = "#00000000"_color;

      solid() = default;
      solid(color::Color _c)
          : c(_c) {}

      void apply_to_source(pixelmap_editor_t& editor, double opacity) const override {
        editor->set_source_rgba(c.r, c.g, c.b, c.a * opacity);
      }

      color::Color sample(int x, int y) const override {
        return c;
      }

      [[nodiscard]]
      base_i* make_copy() const override {
        return new solid{*this};
      }

      paint_type_e get_type() const override {
        return SOLID;
      }

      bool operator==(const base_i& rhl) const override {
        if (rhl.get_type() != get_type())
          return false;
        return c == ((const solid*)&rhl)->c;
      }
    };

    namespace gradient {
      struct base_i: public paint::base_i {
        std::vector<Cairo::ColorStop> color_stops{};
        int                           x0, y0;
        int                           x1, y1;
      };

      struct linear: public gradient::base_i {
        linear(
          std::pair<int, int>                                 start,
          std::pair<int, int>                                 end,
          const std::vector<std::pair<double, color::Color>>& _color_stops
        ) {
          x0 = start.first;
          y0 = start.second;
          x1 = end.first;
          y1 = end.second;
          for (auto& item: _color_stops) {
            color_stops.push_back({
              .offset = item.first,
              .red    = item.second.r,
              .green  = item.second.g,
              .blue   = item.second.b,
              .alpha  = item.second.a,
            });
          }
        }

        void apply_to_source(pixelmap_editor_t& editor, double opacity) const override {
          auto lg = Cairo::LinearGradient::create(x0, y0, x1, y1);
          for (auto& item: color_stops) {
            lg->add_color_stop_rgba(item.offset, item.red, item.green, item.blue, item.alpha);
          }
          editor->set_source(lg);
        }
        color::Color sample(int x, int y) const override {
          return "#00000000"_color;
        }

        [[nodiscard]]
        base_i* make_copy() const override {
          return new linear{*this};
        }

        paint_type_e get_type() const override {
          return GRADIENT_LINEAR;
        }

        bool operator==(const paint::base_i& rhl_) const override {
          if (rhl_.get_type() != get_type())
            return false;
          linear& rhl = *((linear*)&rhl_);
          return x0 == rhl.x0 && x1 == rhl.x1 && y0 == rhl.y0 && y1 == rhl.y1;
        }
      };
      struct radial: public gradient::base_i {
        int r0, r1;

        radial(
          std::pair<int, int>                                 start,
          int                                                 r0,
          std::pair<int, int>                                 end,
          int                                                 r1,
          const std::vector<std::pair<double, color::Color>>& _color_stops
        ) {
          x0       = start.first;
          y0       = start.second;
          this->r0 = r0;
          x1       = end.first;
          y1       = end.second;
          this->r1 = r1;
          for (auto& item: _color_stops) {
            color_stops.push_back({
              .offset = item.first,
              .red    = item.second.r,
              .green  = item.second.g,
              .blue   = item.second.b,
              .alpha  = item.second.a,
            });
          }
        }

        void apply_to_source(pixelmap_editor_t& editor, double opacity) const override {
          auto lg = Cairo::RadialGradient::create(x0, y0, r0, x1, y1, r1);
          for (auto& item: color_stops) {
            lg->add_color_stop_rgba(item.offset, item.red, item.green, item.blue, item.alpha);
          }
          editor->set_source(lg);
        }
        color::Color sample(int x, int y) const override {
          return "#00000000"_color;
        }

        [[nodiscard]]
        base_i* make_copy() const override {
          return new radial{*this};
        }

        paint_type_e get_type() const override {
          return GRADIENT_RADIAL;
        }

        bool operator==(const paint::base_i& rhl_) const override {
          if (rhl_.get_type() != get_type())
            return false;
          radial& rhl = *((radial*)&rhl_);
          return x0 == rhl.x0 && x1 == rhl.x1 && y0 == rhl.y0 && y1 == rhl.y1 && r0 == rhl.r0
                 && r1 == rhl.r1;
        }
      };
    } // namespace gradient

    namespace pixelmap {
      struct base_i: public paint::base_i {};
    } // namespace pixelmap

    // using type = std::variant<
    // solid,
    // gradient::linear,
    // gradient::radial
    // >;

    struct type {
      using enum paint_type_e;
      paint_type_e            paint_type = SOLID;
      std::unique_ptr<base_i> paint_data = std::make_unique<solid>();

      type(decltype(paint_type) type_, std::unique_ptr<base_i>&& paint_data_)
          : paint_type(type_),
            paint_data(std::move(paint_data_)) {}

      type(const type& rhl)
          : paint_type(rhl.paint_type),
            paint_data(rhl.paint_data->make_copy()) {}
      type& operator=(const type& rhl) {
        this->paint_type    = rhl.paint_type;
        auto new_paint_data = std::unique_ptr<base_i>(rhl.paint_data->make_copy());
        std::swap(this->paint_data, new_paint_data);
        return *this;
      };

      bool operator==(const type& rhl) const {
        return paint_type == rhl.paint_type && (*paint_data) == (*rhl.paint_data);
      }
    };
  } // namespace paint

  struct attribute_i {};

#define VG_ATTRIBUTE(TYPE, NAME, DEFAULT)                                                          \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    inline E& NAME(TYPE& _##NAME##_) {                                                             \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(TYPE&& _##NAME##_) {                                                            \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    TYPE _##NAME = DEFAULT;                                                                        \
  }

#define VG_ATTRIBUTE_AUTO(TYPE, NAME, DEFAULT)                                                     \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    inline E& NAME(auto& _##NAME##_) {                                                             \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(auto&& _##NAME##_) {                                                            \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    TYPE _##NAME = DEFAULT;                                                                        \
  }

#define VG_ATTRIBUTE_PAINT(NAME)                                                                   \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    inline E& NAME(vg::paint::type& _##NAME##_) {                                                  \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::type&& _##NAME##_) {                                                 \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::solid& _##NAME##_) {                                                 \
      this->_##NAME.paint_data = std::make_unique<vg::paint::solid>(_##NAME##_);                   \
      this->_##NAME.paint_type = vg::paint::type::SOLID;                                           \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::solid&& _##NAME##_) {                                                \
      this->_##NAME.paint_data = std::make_unique<vg::paint::solid>(_##NAME##_);                   \
      this->_##NAME.paint_type = vg::paint::type::SOLID;                                           \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::gradient::linear& _##NAME##_) {                                      \
      this->_##NAME.paint_data = std::make_unique<vg::paint::gradient::linear>(_##NAME##_);        \
      this->_##NAME.paint_type = vg::paint::type::GRADIENT_LINEAR;                                 \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::gradient::linear&& _##NAME##_) {                                     \
      this->_##NAME.paint_data = std::make_unique<vg::paint::gradient::linear>(_##NAME##_);        \
      this->_##NAME.paint_type = vg::paint::type::GRADIENT_LINEAR;                                 \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::gradient::radial& _##NAME##_) {                                      \
      this->_##NAME.paint_data = std::make_unique<vg::paint::gradient::radial>(_##NAME##_);        \
      this->_##NAME.paint_type = vg::paint::type::GRADIENT_RADIAL;                                 \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(vg::paint::gradient::radial&& _##NAME##_) {                                     \
      this->_##NAME.paint_data = std::make_unique<vg::paint::gradient::radial>(_##NAME##_);        \
      this->_##NAME.paint_type = vg::paint::type::GRADIENT_RADIAL;                                 \
      return *(E*)this;                                                                            \
    }                                                                                              \
    vg::paint::type _##NAME{vg::paint::type::SOLID, std::make_unique<vg::paint::solid>()};         \
  }

#define VG_ATTRIBUTE_DIMENSION(NAME, DEFAULT)                                                      \
  template <typename E>                                                                            \
  struct attr_##NAME: public attribute_i {                                                         \
    template <typename UNIT, typename T>                                                           \
    inline E& NAME(const quantify::quantity_t<UNIT, T>& _##NAME##_) {                         \
      this->_##NAME = _##NAME##_.value_as_base_unit();                                             \
      return *(E*)this;                                                                            \
    }                                                                                              \
    template <typename UNIT, typename T>                                                           \
    inline E& NAME(quantify::quantity_t<UNIT, T>&& _##NAME##_) {                              \
      this->_##NAME = _##NAME##_.value_as_base_unit();                                             \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(const int& _##NAME##_) {                                                        \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    inline E& NAME(int&& _##NAME##_) {                                                             \
      this->_##NAME = _##NAME##_;                                                                  \
      return *(E*)this;                                                                            \
    }                                                                                              \
    int _##NAME = DEFAULT;                                                                         \
  }

  //! @brief id
  VG_ATTRIBUTE(std::string, id, " ");
  //! @brief class
  VG_ATTRIBUTE(std::vector<std::string>, style_class, std::vector<std::string>{});

  //! @brief x  - x-axis coordinate
  VG_ATTRIBUTE_DIMENSION(x, 0);
  //! @brief y  - y-axis coordinate
  VG_ATTRIBUTE_DIMENSION(y, 0);
  //! @brief r  - radius
  VG_ATTRIBUTE_DIMENSION(r, 0);
  //! @brief rx - radius in x-axis
  VG_ATTRIBUTE_DIMENSION(rx, 0);
  //! @brief ry - radius in y-axis
  VG_ATTRIBUTE_DIMENSION(ry, 0);
  //! @brief x1 - 1st point x-axis coordinate
  VG_ATTRIBUTE_DIMENSION(x1, 0);
  //! @brief y1 - 1st point y-axis coordinate
  VG_ATTRIBUTE_DIMENSION(y1, 0);
  //! @brief x2 - 2nd point x-axis coordinate
  VG_ATTRIBUTE_DIMENSION(x2, 0);
  //! @brief y2 - 2nd point y-axis coordinate
  VG_ATTRIBUTE_DIMENSION(y2, 0);
  //! @brief cx - center x-axis coordinate
  VG_ATTRIBUTE_DIMENSION(cx, 0);
  //! @brief cy - center y-axis coordinate
  VG_ATTRIBUTE_DIMENSION(cy, 0);
  //! @brief rotate - angle of rotation in degrees
  VG_ATTRIBUTE(double, rotate, 0);
  //! @brief pivot_x - rotate pivot x-axis coordinate
  VG_ATTRIBUTE_DIMENSION(pivot_x, 0);
  //! @brief pivot_y - rotate pivot y-axis coordinate
  VG_ATTRIBUTE_DIMENSION(pivot_y, 0);
  //! @brief a1 - 1st angle in arcs in degrees
  VG_ATTRIBUTE(double, a1, 0);
  //! @brief a1 - 2st angle in arcs in degrees
  VG_ATTRIBUTE(double, a2, 0);
  //! @brief rotate - angle of rotation in degrees
  VG_ATTRIBUTE(double, scale_x, 1);
  //! @brief rotate - angle of rotation in degrees
  VG_ATTRIBUTE(double, scale_y, 1);

  using point_list_t = std::vector<std::array<int, 2>>;
  //! @brief points
  VG_ATTRIBUTE(point_list_t, points, point_list_t{});

  //! @brief d - path str
  VG_ATTRIBUTE(std::string, path_str, std::string{});

  //! @brief width
  VG_ATTRIBUTE_DIMENSION(w, 0);
  //! @brief height
  VG_ATTRIBUTE_DIMENSION(h, 0);

  //! @brief hidden - replaces display="none" from SVG
  VG_ATTRIBUTE(bool, hidden, 1.0);
  //! @brief opacity
  VG_ATTRIBUTE(float, opacity, 1.0);

  // ?* STROKE ATTRIBUTES
  //! @brief stroke
  VG_ATTRIBUTE_PAINT(stroke);
  //! @brief stroke-dasharray
  VG_ATTRIBUTE(std::valarray<double>, stroke_dasharray, std::valarray<double>{});
  //! @brief stroke-dashoffset
  VG_ATTRIBUTE(double, stroke_dashoffset, 0.0);

  // enum class stroke_linecap_e {
  //   BUTT,
  //   ROUND,
  //   SQUARE,
  // };
  using stroke_linecap_e = Cairo::Context::LineCap;

  //! @brief stroke-linecap
  VG_ATTRIBUTE(stroke_linecap_e, stroke_linecap, stroke_linecap_e::BUTT);

  // enum class stroke_linejoin_e {
  //   ARCS,
  //   BEVEL,
  //   MITER,
  //   MITER_CLIP,
  //   ROUND,
  // };
  using stroke_linejoin_e = Cairo::Context::LineJoin;

  //! @brief stroke-linejoin
  VG_ATTRIBUTE(stroke_linejoin_e, stroke_linejoin, stroke_linejoin_e::MITER);

  //! @brief stroke-miterlimit
  VG_ATTRIBUTE(int, stroke_miterlimit, 4);
  //! @brief stroke-opacity
  VG_ATTRIBUTE(float, stroke_opacity, 1.0);
  //! @brief stroke-width
  VG_ATTRIBUTE_DIMENSION(stroke_width, 0);

  // ?* TEXT ATTRIBUTES
  //! @brief font-family
  VG_ATTRIBUTE(std::string, font_family, "default");
  //! @brief font-size
  VG_ATTRIBUTE(int, font_size, 12);
  // brief font-size-adjust
  // VG_ATTRIBUTE(double, font_size_adjust, 0.0); What is this?
  //! @brief font-stretch
  VG_ATTRIBUTE(double, font_stretch, 1.0); //! UNUSED

  using font_style_e = Cairo::ToyFontFace::Slant;
  //! @brief font-style
  VG_ATTRIBUTE(font_style_e, font_style, font_style_e::NORMAL);

  using font_weight_e = Cairo::ToyFontFace::Weight;
  //! @brief font-weight
  VG_ATTRIBUTE(font_weight_e, font_weight, font_weight_e::NORMAL);

  enum class text_anchor_e {
    START,
    MIDDLE,
    END,
  };

  //! @brief text-anchor
  VG_ATTRIBUTE(text_anchor_e, text_anchor, text_anchor_e::START);

  struct text_decoration_t {
    bool underline      = false;
    bool overline       = false;
    bool strike_through = false;
  };

  //! @brief text-decoration
  VG_ATTRIBUTE(text_decoration_t, text_decoration, text_decoration_t{});

  // ?* COLOR ATTRIBUTES
  // brief color
  //! @brief fill
  VG_ATTRIBUTE_PAINT(fill);
  //! @brief fill-opacity
  VG_ATTRIBUTE(double, fill_opacity, 1.0);

  using fill_rule_e = Cairo::Context::FillRule;

  //! @brief fill-rule
  VG_ATTRIBUTE(fill_rule_e, fill_rule, fill_rule_e::WINDING);

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
                       attr_stroke_linecap<T> {
  protected:
    void apply_stroke(pixelmap_editor_t& editor) const {
      editor->set_line_width(this->_stroke_width);
      editor->set_line_cap(this->_stroke_linecap);
      editor->set_line_join(this->_stroke_linejoin);
      editor->set_dash(this->_stroke_dasharray, this->_stroke_dashoffset);
      editor->set_miter_limit(this->_stroke_miterlimit);
    }

    void set_source_to_stroke(pixelmap_editor_t& editor) const {
      this->_stroke.paint_data->apply_to_source(editor, this->_stroke_opacity);
    }

    [[nodiscard]]
    color::Color sample_stroke(int x, int y) const {
      return this->_stroke.paint_data->sample(x, y);
    }
  };

  template <typename T>
  struct attrs_fill: attr_fill<T>, attr_fill_opacity<T>, attr_fill_rule<T> {
  protected:
    void apply_fill(pixelmap_editor_t& editor) const {
      editor->set_fill_rule(this->_fill_rule);
    }

    void set_source_to_fill(pixelmap_editor_t& editor) const {
      this->_fill.paint_data->apply_to_source(editor, this->_fill_opacity);
    }

    [[nodiscard]]
    color::Color sample_fill(int x, int y) const {
      return this->_fill.paint_data->sample(x, y);
    }
  };

  template <typename T>
  struct attrs_font: attr_font_family<T>,
                     attr_font_size<T>,
                     attr_font_stretch<T>,
                     attr_font_style<T>,
                     attr_font_weight<T> {
  protected:
    void apply_font(pixelmap_editor_t& editor) const {
      // TODO - select_font_face is aparently a 'toy' implementation, shouldn't use it!
      editor->select_font_face(this->_font_family, this->_font_style, this->_font_weight);
      editor->set_font_size(this->_font_size);
    }
  };

  template <typename T>
  struct attrs_core:
      // attr_id<T>,
      // attr_style_class<T>,
      attr_hidden<T>,
      attr_opacity<T> {};
}


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

