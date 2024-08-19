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
  export COMPONENT(plot, {
    std::vector<data_series_t> series{};
  };
  axis_t<plot>     top_axis     {*this, {1,0}, {0,-1}, {1,0}, false, 20, 50};
  axis_t<plot>     bottom_axis  {*this, {1,0}, {0,1}, {1,0}, true, 20};
  axis_t<plot>     left_axis    {*this, {0,-1}, {-1,0}, {1,0}, true, 30, 60};
  axis_t<plot>     right_axis   {*this, {0,-1}, {1,0}, {1,0}, false, 25, 55};
  grid_t<plot>     grid         {*this};
  view_map_t<plot> series       {*this};

  explicit plot(std::initializer_list<data_series_t>&& series_): cyd::ui::components::component_t<EventHandlerplot,plot>(), props(series_) {
    for (std::size_t i = 0; i < series_.size(); ++i) {
      (void)series[i];
    }
  }

  plot(const plot& rhl): cyd::ui::components::component_t<EventHandlerplot,plot>() {
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
      auto &instance = *component_instance();

      int title_h = 0;

      int axis_title_size = 25;

      int top_axis_h = instance.top_axis.show_? 40: 0;
      int bottom_axis_h = instance.bottom_axis.show_? 40: 0;
      int left_axis_w = instance.left_axis.show_? 50: 0;
      int right_axis_w = instance.right_axis.show_? 50: 0;

      if (top_axis_h > 0 && !instance.top_axis.title_.empty()) {
        top_axis_h += axis_title_size;
      }
      if (bottom_axis_h > 0 && !instance.bottom_axis.title_.empty()) {
        bottom_axis_h += axis_title_size;
      }
      if (left_axis_w > 0 && !instance.left_axis.title_.empty()) {
        left_axis_w += axis_title_size;
      }
      if (right_axis_w > 0 && !instance.right_axis.title_.empty()) {
        right_axis_w += axis_title_size;
      }

      std::vector<cyd::ui::components::component_holder_t> result{};

      result.push_back(instance.grid.build_component(left_axis_w, title_h + top_axis_h, $cw() - left_axis_w - right_axis_w, $ch() - title_h - top_axis_h - bottom_axis_h));
      for (auto& serie : instance.series.build_component(left_axis_w, title_h + top_axis_h, $cw() - left_axis_w - right_axis_w, $ch() - title_h - top_axis_h - bottom_axis_h)) {
        result.push_back(serie);
      }
      if (top_axis_h > 0) {
        result.push_back(instance.top_axis.build_component(left_axis_w, title_h, $cw() - left_axis_w - right_axis_w, top_axis_h));
      }
      if (bottom_axis_h > 0) {
        result.push_back(instance.bottom_axis.build_component(left_axis_w, $ch() - bottom_axis_h, $cw() - left_axis_w - right_axis_w, bottom_axis_h));
      }
      if (left_axis_w > 0) {
        result.push_back(instance.left_axis.build_component(0, title_h + top_axis_h, left_axis_w, $ch() - title_h - top_axis_h - bottom_axis_h));
      }
      if (right_axis_w > 0) {
        result.push_back(instance.right_axis.build_component($cw() - right_axis_w, title_h + top_axis_h, right_axis_w, $ch() - title_h - top_axis_h - bottom_axis_h));
      }

      return result;
    }

    FRAGMENT {
      fragment.append(vg::rect { }
                      .w($cw()).h($ch())
                      .fill("#000000"_color)
      );
    }
  };
}
