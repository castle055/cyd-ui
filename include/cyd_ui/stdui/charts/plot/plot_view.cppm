//
// Created by castle on 8/11/24.
//

module;
#include "cyd_ui/components/component_macros.h"

export module cydui.std.charts.plot:view;

import cydui;

import fabric.linalg;
import fabric.logging;

import :series;

using la = with_precision<double>;

namespace charts {
  COMPONENT(PlotView, {
    const data_series_t& series;
    la::scalar min_x = 0, max_x = 1;
    la::scalar min_y = 0, max_y = 1;
    bool show_data_points = true;
    bool show_lines = true;
    std::function<void(vg::vg_fragment_t&, int x, int y)> data_point_fragment = [](vg::vg_fragment_t&, int, int){};
    std::function<void(vg::vg_fragment_t&, int x1, int y1, int x2, int y2)> line_fragment = [](vg::vg_fragment_t&, int, int, int, int){};
  }) {
    ON_REDRAW {
      return {};
    }

    FRAGMENT {
      const auto size = props->series.size();

      bool out_of_frame = false;
      auto data_point   = props->series[0];
      auto prev         = in_screen_space(data_point);
      if (data_point[0] < props->min_x
          || data_point[0] > props->max_x
          || data_point[1] < props->min_y
          || data_point[1] > props->max_y) {
        out_of_frame = true;
      }
      if (props->show_lines) {
        for (std::size_t i = 1; i < size; ++i) {
          data_point = props->series[i];
          const auto current = in_screen_space(data_point);
          if ((current - prev).mag2() > 2) {
            if (out_of_frame
                && (data_point[0] >= props->min_x
                     && data_point[0] <= props->max_x
                     && data_point[1] >= props->min_y
                     && data_point[1] <= props->max_y)
            ) {
              out_of_frame = false;
            }

            if (!out_of_frame) {
              props->line_fragment(fragment, prev[0], prev[1], current[0], current[1]);
            }
            prev = current;

            if (!out_of_frame
                && (data_point[0] < props->min_x
                    || data_point[0] > props->max_x
                    || data_point[1] < props->min_y
                    || data_point[1] > props->max_y)
            ) {
              out_of_frame = true;
            }
          }
        }
      }
      if (props->show_data_points) {
        for (std::size_t i = 0; i < size; ++i) {
          data_point = props->series[i];
          auto current = in_screen_space(data_point);
          if (data_point[0] >= props->min_x
              && data_point[0] <= props->max_x
              && data_point[1] >= props->min_y
              && data_point[1] <= props->max_y) {
            props->data_point_fragment(fragment, current[0], current[1]);
          }
        }
      }
    }

  private:
    la::vec<2> in_screen_space(const la::vec<2>& point) const {
      return {
        (la::scalar)$cw().val() * ((point[0] - props->min_x) / (props->max_x - props->min_x)),
        (la::scalar)$ch().val() - (la::scalar)$ch().val() * ((point[1] - props->min_y) / (props->max_y - props->min_y)),
      };
    }
  };

  export template <class C>
  struct view_map_t;

  template <class C>
  struct view_t {
    explicit view_t(C& plot, const std::size_t index)
      : ref_(&plot), index_(index) {
    }

    view_t(const view_t& rhl)
      : ref_(rhl.ref_) {
      index_ = rhl.index_;
      show_data_points_ = rhl.show_data_points_;
      show_line_ = rhl.show_line_;
    }

    view_t(view_t&& rhl) = delete;

    C& show_data_points(const bool show_points) {
      show_data_points_ = show_points;
      return *ref_;
    }

    C& show_line(const bool show_) {
      show_line_ = show_;
      return *ref_;
    }

  private:
    view_t& operator=(const view_t& rhl) {
      index_ = rhl.index_;
      show_data_points_ = rhl.show_data_points_;
      show_line_ = rhl.show_line_;
      return *this;
    }
    view_t& operator=(view_t&& rhl) = delete;

    C& set_ref(C& ref) {
      ref_ = &ref;
      return *ref_;
    }

    bool exists_in_plot() const {
      return ref_->props.series.size() > index_;
    }

    PlotView build_component(const auto& x, const auto& y, const auto& w, const auto& h) const {
      PlotView pa {
        {
          .series = ref_->props.series[index_],
          .min_x = ref_->bottom_axis.min_value_,
          .max_x = ref_->bottom_axis.max_value_,
          .min_y = ref_->left_axis.min_value_,
          .max_y = ref_->left_axis.max_value_,
          .show_data_points = show_data_points_,
          .show_lines = show_line_,
          .data_point_fragment = [](vg::vg_fragment_t& fragment, int x, int y) {
            fragment.append(vg::circle { }
                            .cx(x).cy(y)
                            .r(3)
                            .fill("#FCAE1E"_color)
                            .stroke_width(1));
          },
          .line_fragment = [](vg::vg_fragment_t& fragment, int x1, int y1, int x2, int y2) {
            fragment.append(vg::line { }
                            .x1(x1).y1(y1)
                            .x2(x2).y2(y2)
                            .stroke("#FCAE1E"_color)
                            .stroke_width(2));
          },
        }
      };
      pa.x(x).y(y).w(w).h(h);
      return pa;
    }

    friend C;
    friend typename C::event_handler_t;
    template <class> friend struct view_map_t;

  private:
    bool show_data_points_ = true;
    bool show_line_ = true;

  private:
    C*         ref_;
    std::size_t index_;
  };

  template <class C>
  view_t(C&) -> view_t<C>;

  template <class C>
  struct view_map_t {
    explicit view_map_t(C& plot)
      : ref_(&plot) {
    }

    view_map_t(const view_map_t& rhl)
      : ref_(rhl.ref_) {
      for (auto &[index, view] : rhl.views_) {
        views_.emplace(index, view);
        views_.at(index).set_ref(*ref_);
      }
    }

    view_map_t(view_map_t&& rhl) = delete;

    view_t<C>& operator[](const std::size_t index) {
      if (!views_.contains(index)) {
        views_.emplace(index, view_t<C> {*ref_, index});
      }
      return views_.at(index);
    }

  private:
    view_map_t& operator=(const view_map_t& rhl) {
      for (auto &[index, view] : rhl.views_) {
        views_.emplace(index, view);
        views_.at(index).set_ref(*ref_);
      }

      return *this;
    }
    view_map_t& operator=(view_map_t&& rhl) = delete;

    C& set_ref(C& ref) {
      ref_ = &ref;
      for (auto &[index, view] : views_) {
        view.set_ref(*ref_);
      }
      return *ref_;
    }

    std::vector<PlotView> build_component(const auto& x, const auto& y, const auto& w, const auto& h) const {
      std::vector<PlotView> result{};
      for (const auto & [index, view] : views_) {
        if (!view.exists_in_plot()) { continue; }
        result.push_back(view.build_component(x, y, w, h));
      }
      return result;
    }

    friend C;
    friend typename C::event_handler_t;

  private:
    std::map<std::size_t, view_t<C>> views_{};
  private:
    C* ref_;
  };

  export template <class C>
  view_map_t(C&) -> view_map_t<C>;

}
