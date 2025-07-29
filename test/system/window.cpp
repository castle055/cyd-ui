//
// Created by castle on 8/15/24.
//

#include "cyd_ui/macros.h"
#include "gtest/gtest.h"

import fabric.logging;
import fabric.main;
import cydui;
import cydui.platform.render.cairo;
import cydui.platform.window.sdl3;

using namespace std::chrono_literals;

COMPONENT (TestComponent, {
             int         a;
             float       fasdfasdfasdfasdf;
             double      afsdfasdfasdfasdfasdfasdfassd;
             std::string fdasfasdfasdf;
           }) {
  CHILDREN {
    return {};
  }
  FRAGMENT {
    return {
      cydui::elements::Circle {} //
        .cx(200_px)
        .cy(200_px)
        .r(200_px)
        .fill(cydui::paints::Solid {"#00FF00"_color}),
    };
  }
};

TEST(
  Window,
  Creation) {
  fabric::runtime::main([] -> fabric::task<int> {
    LOG::INIT {}
      .TARGET(
        LOG::TARGETS::STDOUT::BUILDER {}.entry_format(
          "[{entry:level}] ~{entry:thread_name}~ [{entry:path}:{entry:line}:{entry:function}] "
          "{entry:message}"))
      .filter()
      .min_level(INFO)["stdout"];
    cydui::platform::window::SDL3WindowOptions    w_opts {"test-window", 1280, 720};
    cydui::platform::render::CairoRendererOptions r_opts {};

    cydui::Platform::sptr platform = co_await cydui::Platform::make(w_opts, r_opts);

    cydui::UI::sptr ui = co_await cydui::make_ui(platform, TestComponent {});
    co_await ui->show();

    co_await 3h;
    co_return 0;
  });

  // ASSERT_TRUE(result.has_value());
  // EXPECT_EQ(result->x, 10);
  // EXPECT_EQ(result->y, 10);
  // EXPECT_EQ(result->w, 80);
  // EXPECT_EQ(result->h, 80);
}
