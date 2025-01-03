//
// Created by castle on 8/11/24.
//

module;
#include "cyd_ui/components/component_macros.h"

export module cydui.std.charts.plot:axis;

import cydui;

import reflect;
import fabric.linalg;
import fabric.logging;

using la = with_precision<double>;
using as_no_unit = quantify::quantity_t<quantify::no_unit, double>;
// using la = with_precision<quantity_t<no_unit, double>>;
// using la = with_precision<cyd::ui::dimensions::screen_measure>;

namespace charts {
  constexpr la::scalar DEFAULT_AXIS_MIN_VALUE = -1.0;
  constexpr la::scalar DEFAULT_AXIS_MAX_VALUE = +1.0;
  constexpr la::scalar DEFAULT_AXIS_STEP      = 0.1;


  export struct COMPONENT(PlotAxis, {
    la::vec<2> axis_direction{0, 0};
    la::vec<2> label_direction{0, 0};
    la::scalar label_offset{0};
    la::vec<2> label_orientation{0, 0};
    la::scalar min = DEFAULT_AXIS_MIN_VALUE, max = DEFAULT_AXIS_MAX_VALUE, step = DEFAULT_AXIS_STEP;
    bool       major_ticks_show         = false;
    bool       minor_ticks_show         = false;
    unsigned int      minor_ticks_count = 4;
    std::string       title             = "";
    vg::text_anchor_e title_align{vg::text_anchor_e::MIDDLE};
    la::scalar        title_offset{0};
    std::function<cyd::ui::components::component_holder_t(la::scalar)> label_component;
    std::function<void(la::scalar)>                                    move_callback;
    std::function<void()>                                              reset_callback;
  }) {
    ON_REDRAW {
      using cyd::ui::dimensions::screen_measure;
      std::vector<cyd::ui::components::component_holder_t> result{};

      // labels
      auto start     = props.step * std::floor((props.min / props.step));
      auto num_steps = (props.max - start) / props.step;
      for (la::scalar step_i = 0; step_i < num_steps; step_i = step_i + la::scalar{1}) {
        la::scalar X = start + step_i * props.step;
        if (X < props.min || X > props.max) {
          continue;
        }
        auto pos = props.axis_direction * ((X - props.min) / (props.max - props.min));
        result.push_back(props.label_component(X));

        auto  label     = result.back().get_components()[0].second;
        auto  label_dim = label->get_dimensional_relations();
        if (props.axis_direction[0] > la::scalar{0}) {
          label_dim._x = $width * screen_measure{pos[0]} - (label_dim._width / 2_px) +
                         (props.label_direction[0] * props.label_offset);
        } else if (props.axis_direction[0] < la::scalar{0}) {
          label_dim._x = $width * screen_measure{pos[0]} - (label_dim._width / screen_measure{2}) +
                         (props.label_direction[0] * props.label_offset) + $width;
        } else {
          if (props.label_direction[0] > 0) {
            label_dim._x = $width * pos[0] - (label_dim._width / 2) +
                           (props.label_direction[0] * props.label_offset);
          } else {
            label_dim._x = $width * pos[0] - (label_dim._width / 2) +
                           (props.label_direction[0] * props.label_offset) + $width;
          }
        }
        if (props.axis_direction[1] > 0) {
          label_dim._y = $height * pos[1] + (label_dim._height / 2) +
                         (props.label_direction[1] * props.label_offset);
        } else if (props.axis_direction[1] < 0) {
          label_dim._y = $height * pos[1] + (label_dim._height / 2) +
                         (props.label_direction[1] * props.label_offset) + $height;
        } else {
          if (props.label_direction[1] > 0) {
            label_dim._y = $height * pos[1] + (label_dim._height / 2) +
                           (props.label_direction[1] * props.label_offset);
          } else {
            label_dim._y = $height * pos[1] + (label_dim._height / 2) +
                           (props.label_direction[1] * props.label_offset) + $height;
          }
        }
      }
      return result;
    } // namespace charts

    FRAGMENT {
      auto offset_x = 0_px;
      auto offset_y = 0_px;

      if (props.axis_direction[0] < 0) {
        offset_x = $width;
      } else if (props.axis_direction[0] == 0) {
        if (props.label_direction[0] <= 0) {
          offset_x = $width;
        }
      }

      if (props.axis_direction[1] < 0) {
        offset_y = $height;
      } else if (props.axis_direction[1] == 0) {
        if (props.label_direction[1] <= 0) {
          offset_y = $height;
        }
      }

      // draw axis_line
      auto x1 = 0_px;
      auto y1 = 0_px;
      auto x2 =
        as_no_unit{props.axis_direction[0]} * ($height / as_no_unit{props.axis_direction[1]});
      auto y2 =
        as_no_unit{props.axis_direction[1]} * ($width / as_no_unit{props.axis_direction[0]});
      if (y2 > $height) {
        y2 = $height;
      } else if (y2 < -$height) {
        y2 = -$height;
      } else if (x2 > $width) {
        x2 = $width;
      } else if (x2 < -$width) {
        x2 = -$width;
      } else {
        fragment.append(
          vg::line{}.x1(x1).y1(y1).x2($width).y2($height).stroke_width(2).stroke("#FFFFFF"_color)
        );
      }
      fragment.append(vg::line{}
                        .x1(x1 + offset_x)
                        .y1(y1 + offset_y)
                        .x2(x2 + offset_x)
                        .y2(y2 + offset_y)
                        .stroke_width(2)
                        .stroke("#777777"_color));
      const auto line_length = std::sqrt(la::vec<2>{(x2 - x1).value, (y2 - y1).value}.mag2());

      // draw ticks
      auto start = props.step * std::floor((props.min / props.step));
      if (props.major_ticks_show) {
        for (la::scalar X = start; X < props.max; X += props.step) {
          auto pos = props.axis_direction * ((X - props.min) / (props.max - props.min));

          x1 = cyd::ui::dimensions::screen_measure{line_length * pos[0]} + offset_x;
          y1 = cyd::ui::dimensions::screen_measure{line_length * pos[1]} + offset_y;
          if (X >= props.min && X <= props.max) {
            fragment.append(vg::line{}
                              .x1(x1)
                              .y1(y1)
                              .x2(
                                x1 +
                                cyd::ui::dimensions::screen_measure{
                                  props.label_direction[0] * props.label_offset * 0.25
                                }
                              )
                              .y2(
                                y1 +
                                cyd::ui::dimensions::screen_measure{
                                  props.label_direction[1] * props.label_offset * 0.25
                                }
                              )
                              // .stroke_width(1).stroke("#555555"_color));
                              .stroke_width(2)
                              .stroke("#777777"_color));
          }

          if (props.minor_ticks_show) {
            la::scalar minor_step = props.step / (props.minor_ticks_count + 1.0);
            for (la::scalar Y  = minor_step; Y <= props.step - minor_step && (X + Y) < props.max;
                 Y            += minor_step) {
              pos = props.axis_direction * (((X + Y) - props.min) / (props.max - props.min));

              x1 = cyd::ui::dimensions::screen_measure{line_length * pos[0]} + offset_x;
              y1 = cyd::ui::dimensions::screen_measure{line_length * pos[1]} + offset_y;
              if ((X + Y) >= props.min && (X + Y) <= props.max) {
                fragment.append(vg::line{}
                                  .x1(x1)
                                  .y1(y1)
                                  .x2(
                                    x1 +
                                    cyd::ui::dimensions::screen_measure{
                                      props.label_direction[0] * props.label_offset * 0.15
                                    }
                                  )
                                  .y2(
                                    y1 +
                                    cyd::ui::dimensions::screen_measure{
                                      props.label_direction[1] * props.label_offset * 0.15
                                    }
                                  )
                                  // .stroke_width(1).stroke("#555555"_color));
                                  .stroke_width(1)
                                  .stroke("#777777"_color));
              }
            }
          }
        }
      }

      // draw title
      if (!props.title.empty()) {
        auto title =
          vg::text{props.title}.font_family("Helvetica").font_size(14).fill("#EEEEEE"_color);
        auto footprint = title.get_footprint();
        title.pivot_x(footprint.w / 2);
        title.pivot_y(footprint.h / 2);

        double ratio;
        int    text_offset_x;
        int    text_offset_y = footprint.h / 2;
        switch (props.title_align) {
          case vg::text_anchor_e::START:
            ratio         = 0.0;
            text_offset_x = 0;
            break;
          case vg::text_anchor_e::MIDDLE:
            ratio         = 0.5;
            text_offset_x = footprint.w / 2;
            break;
          case vg::text_anchor_e::END:
            ratio         = 1.0;
            text_offset_x = footprint.w;
            break;
        }
        auto pos = (props.axis_direction * ratio * line_length) +
                   (props.label_direction * props.title_offset);
        auto angle =
          std::atan2(props.axis_direction[1], props.axis_direction[0]) * 180.0 / std::numbers::pi;

        title.x(cyd::ui::dimensions::screen_measure{pos[0] - text_offset_x} + offset_x);
        title.y(cyd::ui::dimensions::screen_measure{pos[1] + text_offset_y} + offset_y);
        title.rotate(angle);
        fragment.append(title);
      }
    }

    ON_SCROLL {
      if (dy.value > 0) {
        props.move_callback(0.1);
        state.parent()->mark_dirty();
      } else if (dy.value < 0) {
        props.move_callback(-0.1);
        state.parent()->mark_dirty();
      }
    }

    ON_BUTTON_RELEASE {
      if (button == Button::WHEEL) {
        props.reset_callback();
        state.parent()->mark_dirty();
      }
    }
  };
  export struct I {
    struct WILL {
      struct MURDER {
        int murder_you;
      };
    };
  };


  export struct COMPONENT(
    NumericAxisLabel, { la::scalar value = 0; };
    static cyd::ui::components::component_holder_t builder(const la::scalar value) {
      return NumericAxisLabel{{value}};
    }
  ) {
    ON_REDRAW {
      const auto label_footprint = label_fragment().get_footprint();
      $width                     = cyd::ui::dimensions::screen_measure{(double)label_footprint.w};
      $height                    = cyd::ui::dimensions::screen_measure{(double)label_footprint.h};
      return {};
    }

    FRAGMENT {
      fragment.append(label_fragment());
    }

  private:
    vg::text label_fragment() const {
      return vg::text{std::format("{:.1f}", props.value)}
        .font_size(14)
        .font_family("Helvetica")
        .fill("#CCCCCC"_color);
    }
  };

  export struct COMPONENT(
    PiRatioAxisLabel, { la::scalar value = 0; };
    static cyd::ui::components::component_holder_t builder(const la::scalar value) {
      return PiRatioAxisLabel{{value}};
    }
  ) {
    ON_REDRAW {
      const auto label_footprint = label_fragment().get_footprint();
      $width                     = cyd::ui::dimensions::screen_measure{(double)label_footprint.w};
      $height                    = cyd::ui::dimensions::screen_measure{(double)label_footprint.h};
      return {};
    }

    FRAGMENT {
      fragment.append(label_fragment());
    }

  private:
    vg::text label_fragment() const {
      auto pi_ratio = props.value / std::numbers::pi;
      return vg::text{std::format("{:.2f}π", pi_ratio)}
        .font_size(14)
        .font_family("Helvetica")
        .fill("#CCCCCC"_color);
    }
  };

  export template <class C>
  struct axis_t {
    explicit axis_t(
      C&               plot,
      const la::vec<2> axis_direction,
      const la::vec<2> label_direction,
      const la::vec<2> label_orientation,
      bool             default_show,
      la::scalar       default_label_offset = 30,
      la::scalar default_title_offset       = 45

    )
        : show_(default_show),
          label_offset_(default_label_offset),
          title_offset_(default_title_offset),
          ref_(&plot),
          axis_direction_(axis_direction),
          label_direction_(label_direction),
          label_orientation_(label_orientation) {
      initial_min_value_ = min_value_;
      initial_max_value_ = max_value_;
      initial_step_      = step_;
    }

    C& show(const bool show_axis) {
      show_ = show_axis;
      return *ref_;
    }

    C& bounds(const la::scalar min, const la::scalar max, const la::scalar step) {
      auto_scale_            = false;
      auto_scale_bound_zero_ = false;
      min_value_             = min;
      max_value_             = max;
      step_                  = step;
      initial_min_value_     = min_value_;
      initial_max_value_     = max_value_;
      initial_step_          = step_;
      return *ref_;
    }

    C& bounds_auto(const bool bind_zero = false) {
      auto_scale_            = true;
      auto_scale_bound_zero_ = bind_zero;
      return *ref_;
    }

    C& major_ticks_show(const bool show_ticks) {
      major_ticks_show_ = show_ticks;
      return *ref_;
    }

    C& minor_ticks_show(const bool show_ticks) {
      minor_ticks_show_ = show_ticks;
      return *ref_;
    }

    C& minor_ticks_count(const unsigned int tick_count) {
      minor_ticks_count_ = tick_count;
      return *ref_;
    }

    C& title(const std::string& title_str) {
      title_ = title_str;
      return *ref_;
    }

    C& title_align(const vg::text_anchor_e align) {
      title_align_ = align;
      return *ref_;
    }

    C& title_offset(const la::scalar offset) {
      title_offset_ = offset;
      return *ref_;
    }


    C& label_component(
      const std::function<cyd::ui::components::component_holder_t(la::scalar)>& builder
    ) {
      label_builder_ = [&](const la::scalar value) { return builder(value); };
      return *ref_;
    }

    template <typename L>
    C& label_component() {
      label_builder_ = [&](const la::scalar value) { return L{{value}}; };
      return *ref_;
    }

  private:
    axis_t& operator=(const axis_t& rhl) {
      show_                  = rhl.show_;
      auto_scale_            = rhl.auto_scale_;
      auto_scale_bound_zero_ = rhl.auto_scale_bound_zero_;
      min_value_             = rhl.min_value_;
      max_value_             = rhl.max_value_;
      step_                  = rhl.step_;
      initial_min_value_     = rhl.initial_min_value_;
      initial_max_value_     = rhl.initial_max_value_;
      initial_step_          = rhl.initial_step_;
      axis_direction_        = rhl.axis_direction_;
      label_direction_       = rhl.label_direction_;
      label_orientation_     = rhl.label_orientation_;
      label_builder_         = rhl.label_builder_;
      label_offset_          = rhl.label_offset_;
      title_                 = rhl.title_;
      title_align_           = rhl.title_align_;
      title_offset_          = rhl.title_offset_;
      return *this;
    }

    PlotAxis build_component(auto&& x, auto&& y, auto&& w, auto&& h) {
      PlotAxis pa{
        {.axis_direction    = axis_direction_,
         .label_direction   = label_direction_,
         .label_offset      = label_offset_,
         .label_orientation = label_orientation_,
         .min               = min_value_,
         .max               = max_value_,
         .step              = step_,
         .major_ticks_show  = major_ticks_show_,
         .minor_ticks_show  = minor_ticks_show_,
         .minor_ticks_count = minor_ticks_count_,
         .title             = title_,
         .title_align       = title_align_,
         .title_offset      = title_offset_,
         .label_component   = label_builder_,
         .move_callback =
           [&](const la::scalar dx) {
             min_value_ += dx;
             max_value_ += dx;
           },
         .reset_callback =
           [&] {
             min_value_ = initial_min_value_;
             max_value_ = initial_max_value_;
             step_      = initial_step_;
           }}
      };
      pa.x(x).y(y).width(w).height(h);
      return pa;
    }

    friend C;
    friend typename C::event_handler_t;
    template <class>
    friend struct grid_t;
    template <class>
    friend struct view_t;

  private:
    bool              show_{true};
    bool              auto_scale_{true};
    bool              auto_scale_bound_zero_{false};
    la::scalar        min_value_{DEFAULT_AXIS_MIN_VALUE};
    la::scalar        max_value_{DEFAULT_AXIS_MAX_VALUE};
    la::scalar        step_{DEFAULT_AXIS_STEP};
    la::scalar        initial_min_value_{DEFAULT_AXIS_MIN_VALUE};
    la::scalar        initial_max_value_{DEFAULT_AXIS_MAX_VALUE};
    la::scalar        initial_step_{DEFAULT_AXIS_STEP};
    bool              major_ticks_show_{true};
    bool              minor_ticks_show_{false};
    unsigned int      minor_ticks_count_{3};
    la::scalar        label_offset_{30};
    std::string       title_{""};
    vg::text_anchor_e title_align_{vg::text_anchor_e::MIDDLE};
    la::scalar        title_offset_{45};

  private:
    C*                                                                  ref_;
    la::vec<2>                                                          axis_direction_{0, 0};
    la::vec<2>                                                          label_direction_{0, 0};
    la::vec<2>                                                          label_orientation_{0, 0};
    std::function<cyd::ui::components::component_builder_t(la::scalar)> label_builder_{
      [](const la::scalar value) { return NumericAxisLabel{{value}}; }
    };
  };

  export template <class C>
  axis_t(C&) -> axis_t<C>;
}
