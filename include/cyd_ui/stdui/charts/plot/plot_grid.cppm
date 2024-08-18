//
// Created by castle on 8/11/24.
//

module;
#include "cyd_ui/components/component_macros.h"

export module cydui.std.charts.plot:grid;

import cydui;

import fabric.linalg;
import fabric.logging;

import :axis;

using la = with_precision<double>;

namespace charts {
  COMPONENT(PlotGrid, {
    la::scalar min_x = 0, max_x = 1, step_x = 0.1;
    bool major_ticks_x_show = false;
    bool minor_ticks_x_show = false;
    unsigned int minor_ticks_x_count = 4;
    la::scalar min_y = 0, max_y = 1, step_y = 0.1;
    bool major_ticks_y_show = false;
    bool minor_ticks_y_show = false;
    unsigned int minor_ticks_y_count = 4;
  }) {
    ON_REDRAW {
      return {};
    }

    FRAGMENT {
      if (props->major_ticks_x_show) {
        la::scalar x1    = 0;
        auto       start = props->step_x * std::floor((props->min_x / props->step_x));
        for (la::scalar X = start; X < props->max_x; X += props->step_x) {
          auto pos_x = ((X - props->min_x) / (props->max_x - props->min_x));

          x1 = $cw() * pos_x;
          if (X >= props->min_x && X <= props->max_x) {
            fragment.append(vg::line { }
                            .x1(x1).y1(0)
                            .x2(x1).y2($ch())
                            .stroke_width(2).stroke("#555555"_color));
          }

          if (props->minor_ticks_x_show) {
            auto minor_step = props->step_x / (props->minor_ticks_x_count + 1);
            for (la::scalar Y = minor_step; Y <= props->step_x - minor_step && (X + Y) < props->max_x;
              Y += minor_step) {
              pos_x = (((X + Y) - props->min_x) / (props->max_x - props->min_x));

              x1 = $cw() * pos_x;
              if ((X + Y) >= props->min_x && (X + Y) <= props->max_x) {
                fragment.append(vg::line { }
                                .x1(x1).y1(0)
                                .x2(x1).y2($ch())
                                .stroke_width(1).stroke("#555555"_color));
              }
            }
          }
        }
      }
      if (props->major_ticks_y_show) {
        la::scalar y1    = 0;
        auto       start = props->step_y * std::floor((props->min_y / props->step_y));
        for (la::scalar X = start; X < props->max_y; X += props->step_y) {
          auto pos_y = ((X - props->min_y) / (props->max_y - props->min_y));

          y1 = $ch().val() - $ch().val() * pos_y;
          if (X >= props->min_y && X <= props->max_y) {
            fragment.append(vg::line { }
                            .x1(0).y1(y1)
                            .x2($cw()).y2(y1)
                            .stroke_width(2).stroke("#555555"_color));
          }

          if (props->minor_ticks_y_show) {
            auto minor_step = props->step_y / (props->minor_ticks_y_count + 1);
            for (la::scalar Y = minor_step; Y <= props->step_y - minor_step && (X + Y) < props->max_y;
              Y += minor_step) {
              pos_y = (((X + Y) - props->min_y) / (props->max_y - props->min_y));

              y1 = $ch().val() - $ch().val() * pos_y;
              if ((X + Y) >= props->min_y && (X + Y) <= props->max_y) {
                fragment.append(vg::line { }
                                .x1(0).y1(y1)
                                .x2($cw()).y2(y1)
                                .stroke_width(1).stroke("#555555"_color));
              }
            }
          }
        }
      }
    }
  };

  export template <class C>
  struct grid_t {
    explicit grid_t(
      C& plot
    )
      : ref_(&plot), x_axis_ref_(&ref_->bottom_axis), y_axis_ref_(&ref_->left_axis) {
    }

    grid_t(const grid_t& rhl)
      : ref_(rhl.ref_), x_axis_ref_(&ref_->bottom_axis), y_axis_ref_(&ref_->left_axis) {
      x_axis = rhl.x_axis;
      y_axis = rhl.y_axis;
    }

    grid_t(grid_t&& rhl) = delete;

    struct axis_ticks {
      explicit axis_ticks(grid_t& grid): ref_(&grid) { }

      C& major_ticks_show(const bool show_ticks) {
        major_ticks_show_ = show_ticks;
        return *ref_->ref_;
      }

      C& minor_ticks_show(const bool show_ticks) {
        minor_ticks_show_ = show_ticks;
        return *ref_->ref_;
      }

    private:
      friend struct grid_t;

      axis_ticks& operator=(const axis_ticks& rhl) {
        major_ticks_show_ = rhl.major_ticks_show_;
        minor_ticks_show_ = rhl.minor_ticks_show_;
        return *this;
      }

      C& set_ref(grid_t& ref) {
        ref_ = ref;
        return *ref_->ref_;
      }

      bool         major_ticks_show_  = false;
      bool         minor_ticks_show_  = false;

    private:
      grid_t* ref_;
    };

  public:
    axis_ticks x_axis{*this};
    axis_ticks y_axis{*this};
  private:
    friend C;
    friend typename C::event_handler_t;

    grid_t& operator=(const grid_t& rhl) {
      x_axis = rhl.x_axis;
      y_axis = rhl.y_axis;

      return *this;
    }
    grid_t& operator=(grid_t&& rhl) = delete;

    C& set_ref(C& ref) {
      ref_ = &ref;
      x_axis_ref_ = &ref_->bottom_axis;
      y_axis_ref_ = &ref_->left_axis;
      return *ref_;
    }

    PlotGrid build_component(const auto& x, const auto& y, const auto& w, const auto& h) const {
      PlotGrid pa {
        {
          .min_x = x_axis_ref_->min_value_,
          .max_x = x_axis_ref_->max_value_,
          .step_x = x_axis_ref_->step_,
          .major_ticks_x_show = x_axis.major_ticks_show_,
          .minor_ticks_x_show = x_axis.minor_ticks_show_,
          .minor_ticks_x_count = x_axis_ref_->minor_ticks_count_,
          .min_y = y_axis_ref_->min_value_,
          .max_y = y_axis_ref_->max_value_,
          .step_y = y_axis_ref_->step_,
          .major_ticks_y_show = y_axis.major_ticks_show_,
          .minor_ticks_y_show = y_axis.minor_ticks_show_,
          .minor_ticks_y_count = y_axis_ref_->minor_ticks_count_,
        }
      };
      pa.x(x).y(y).w(w).h(h);
      return pa;
    }

    C* ref_;
    axis_t<C>* x_axis_ref_;
    axis_t<C>* y_axis_ref_;
  };

  export template <class C>
  grid_t(C&) -> grid_t<C>;

}
