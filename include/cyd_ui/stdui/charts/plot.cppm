// Copyright (c) 2024, Victor Castillo, All rights reserved.
module;
#include "cyd_ui/components/component_macros.h"

export module cydui.std.charts.plot;

import std;

import cydui;

export import fabric.linalg;
import fabric.logging;

export import :axis;
export import :grid;
export import :series;
export import :view;

export using la = with_precision<double>;

namespace charts {
  export struct COMPONENT(plot, {
    std::vector<data_series_t> series{};
  };
  axis_t<plot>     top_axis     {*this, {1,0}, {0,-1}, {1,0}, false, 20, 50};
  axis_t<plot>     bottom_axis  {*this, {1,0}, {0,1}, {1,0}, true, 20};
  axis_t<plot>     left_axis    {*this, {0,-1}, {-1,0}, {1,0}, true, 30, 60};
  axis_t<plot>     right_axis   {*this, {0,-1}, {1,0}, {1,0}, false, 25, 55};
  grid_t<plot>     grid         {*this};
  view_map_t<plot> series       {*this};

  explicit plot(std::initializer_list<data_series_t>&& series_): cyd::ui::components::component_t<plot>(), props(series_) {
    for (std::size_t i = 0; i < series_.size(); ++i) {
      (void)series[i];
    }
  }

  plot(const plot& rhl): cyd::ui::components::component_t<plot>() {
    props = rhl.props;
    top_axis = rhl.top_axis;
    bottom_axis = rhl.bottom_axis;
    left_axis = rhl.left_axis;
    right_axis = rhl.right_axis;
    grid = rhl.grid;
    grid.set_ref(*this);
    series = rhl.series;
    series.set_ref(*this);
  }
  ) {
    ON_REDRAW {
      using cyd::ui::dimensions::screen_measure;
      double title_h = 0;

      double axis_title_size = 25;

      double top_axis_h = $component.top_axis.show_? 40: 0;
      double bottom_axis_h = $component.bottom_axis.show_? 40: 0;
      double left_axis_w = $component.left_axis.show_? 50: 0;
      double right_axis_w = $component.right_axis.show_? 50: 0;

      if (top_axis_h > 0 && !$component.top_axis.title_.empty()) {
        top_axis_h += axis_title_size;
      }
      if (bottom_axis_h > 0 && !$component.bottom_axis.title_.empty()) {
        bottom_axis_h += axis_title_size;
      }
      if (left_axis_w > 0 && !$component.left_axis.title_.empty()) {
        left_axis_w += axis_title_size;
      }
      if (right_axis_w > 0 && !$component.right_axis.title_.empty()) {
        right_axis_w += axis_title_size;
      }

      std::vector<cyd::ui::components::component_holder_t> result{};

      result.emplace_back($component.grid.build_component(
        screen_measure{left_axis_w},
        screen_measure{title_h + top_axis_h},
        $width - left_axis_w - right_axis_w,
        $height - title_h - top_axis_h - bottom_axis_h
      ));
      for (auto& serie: $component.series.build_component(
             screen_measure{left_axis_w},
             screen_measure{title_h + top_axis_h},
             $width - left_axis_w - right_axis_w,
             $height - title_h - top_axis_h - bottom_axis_h
           )) {
        result.emplace_back(serie);
      }
      if (top_axis_h > 0) {
        result.emplace_back($component.top_axis.build_component(
          screen_measure{left_axis_w},
          screen_measure{title_h},
          $width - left_axis_w - right_axis_w,
          screen_measure{top_axis_h}
        ));
      }
      if (bottom_axis_h > 0) {
        result.emplace_back($component.bottom_axis.build_component(
          screen_measure{left_axis_w},
          $height - bottom_axis_h,
          $width - left_axis_w - right_axis_w,
          screen_measure{bottom_axis_h}
        ));
      }
      if (left_axis_w > 0) {
        result.emplace_back($component.left_axis.build_component(
          0_px,
          screen_measure{title_h + top_axis_h},
          screen_measure{left_axis_w},
          $height - title_h - top_axis_h - bottom_axis_h
        ));
      }
      if (right_axis_w > 0) {
        result.emplace_back($component.right_axis.build_component(
          $width - right_axis_w,
          screen_measure{title_h + top_axis_h},
          screen_measure{right_axis_w},
          $height - title_h - top_axis_h - bottom_axis_h
        ));
      }

      return result;
    }

    FRAGMENT {
      fragment.append(vg::rect { }
                      .w($width).h($height)
                      .fill("#000000"_color)
      );
    }
  };
}
