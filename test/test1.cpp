//
// Created by castle on 8/15/24.
//

#include "cyd_ui/debug/profiling/macros.h"
#include "cyd_ui/macros.h"

#include <tracy/Tracy.hpp>
#include "common.h"

import fabric.logging;
import cydui.std;
import cydui.std.input.text;
import cydui.animations;
import cydui.backends.sdl3;
import cydui.debug.profiling;

using namespace stdui;
using namespace std::chrono_literals;

void setup() {}


struct test_context {
  cydui::Color color{"#FF0000"_color};
};

struct styletype {
  cydui::Color bg{"#ff0000"_color};
};

COMPONENT(
  TestWithContext, //
  { std::string* text; };
  SIGNAL(pressed);
) {
  STATE {
    int val;
  };
  STYLE EXTENDS(styletype) {
    cydui::dimension_t some_dim{0_px};
    double             val{0};
  };
  cydui::use_context<test_context> test_ctx;
  CHILDREN {
    component.width(10_px);
    // style_t sasd{{.background = vg::paint::type::make<vg::paint::solid>("#ffffff"_color)}};
    // $component.background(test_ctx->color);
    // $component.background(style.background);

    // style.some_dim

    return {};
  }

  ON_BUTTON_RELEASE {
    // test_ctx->color = "#00FF00"_color;
    // test_ctx.notify();
    component.get_blueprint().pressed.emit();
  }

  FRAGMENT {
    if (style.val > 0) {
      // LOG::print{DEBUG}("val: {}", style.val);
    }
    fragment.draw<vg::line>()
      .x1(style.val * $width)
      .x2(style.val * $width)
      .y1(0)
      .y2($height)
      .stroke("#0000FF"_color)
      .stroke_width(4);
  }
};
static_assert(cydui::components::StaticBlueprint<TestWithContext>);
static_assert(cydui::components::HasCustomStyleType<TestWithContext>);

static_assert(requires {
  typename cydui::components::event_handler_type<TestWithContext>::style_type;
});
static_assert(cydui::components::StaticBlueprint<TestWithContext> and requires {
  typename cydui::components::event_handler_type<TestWithContext>::style_type;
});

cydui::animation anim{
  {                                                                                       //
   cydui::keyframe::make(1.0, cydui::components::style_type<TestWithContext>{.val = 1.0}) //
     .interp("val", cydui::interp::bezier{{1.0, 0.2}, {0.4, 0.0}})
  },
  cydui::animation_opts{} //
    .easing(cydui::easing::linear)
    .duration(3000ms),
};

COMPONENT(
  TestComponent,
  { std::string* text; }
) {
  cydui::use_context<test_context>     dependency_test_ctx{};
  cydui::provide_context<test_context> test_ctx{};
  CHILDREN {
    return {
      TestWithContext{{}, "animation_target"}
        .x(50_px)
        .y(50_px)
        .width($width / 2)
        .height(200_px)
        .on_pressed([&] {
          auto c = component.find_child("animation_target").value();
          c.animate(anim);
        })
        .border("#00FF00"_color)
        .border_width(2),
      input::text{{props.text}}.x($previous::x).height(30_px).width($previous::width).on_enter([&] {
        std::cout << "ENTER!" << std::endl;
      }),
    };
  }
};

EVENT(ASD){};

COMPONENT(
  SelfSizedComponent,
  {}
){         //
  ON_MOUNT{//
           component.width(300_px).height(250_px);
}
CHILDREN {
  return {};
}
FRAGMENT {
  fragment.draw<vg::circle>().cx(10_px).cy(10_px).r(10_px).fill("#FFFFFF"_color);
}
}
;
COMPONENT(
  AutoSizedComponent,
  {}
){CHILDREN{return {SelfSizedComponent{}};
}
}
;

COMPONENT(
  AutoSizedSuperComponent,
  {}
){CHILDREN{return {AutoSizedComponent{}};
}
}
;

COMPONENT(
  HoverPressTest,
  {}
) {

  ON_BUTTON_PRESS {
    component.get_blueprint().tag("pressed");
    component.mark_dirty();
  }

  ON_BUTTON_RELEASE {
    component.get_blueprint().untag("pressed");
    component.mark_dirty();
  }

  ON_MOUSE_EXIT {
    component.get_blueprint().untag("pressed");
    component.mark_dirty();
  }
};

COMPONENT(
  SizeTestComponent,
  { bool abs = false; }
){         //
  CHILDREN{// component.background("#226622"_color);
           return {
             SelfSizedComponent{}
               .x($height / 2)
               .position(props.abs ? cydui::position_e::ABSOLUTE : cydui::position_e::RELATIVE)
             , //.margin_top(5_px),
             AutoSizedComponent{}.y($previous::bottom_center::y).x($height / 2).padding_top(50_px),
             AutoSizedSuperComponent{}.y($previous::bottom_center::y).x($self::height),
             AutoSizedSuperComponent{}.y($previous::bottom_center::y),
           };
}
}
;

COMPONENT(
  OtherComponent,
  {}
) {
  std::string text1 = "Well, hello there!";
  std::string text2 = "Well, hello there!";
  CHILDREN {
    return {

      // TestComponent{{&text1}}.width($width/2).height($height / 2 - 1),
      // TestComponent{{&text2}}.width($width/2).height($height / 2 - 1).y($height / 2 + 1),
      SizeTestComponent{}
        .y(200_px)
        .overflow_x(cydui::overflow_e::SCROLL)
        .overflow_y(cydui::overflow_e::SCROLL)
        .width(400_px)
        .height(200_px),
      // .border("#FCAE1E"_color)
      // .border_width(10),
      SizeTestComponent{{true}}
        .y(200_px)
        .overflow_y(cydui::overflow_e::GROW)
        .overflow_x(cydui::overflow_e::SCROLL)
        .x(250_px)
        .width(200_px)
        .height(100_px),
      // .border("#FCAE1E"_color)
      // .border_width(2),
      SizeTestComponent{}
        .y(200_px)
        .overflow_x(cydui::overflow_e::GROW)
        .overflow_y(cydui::overflow_e::SCROLL)
        .x(500_px)
        .width(200_px)
        .height(100_px),
      // .border("#FCAE1E"_color)
      // .border_width(4),
      SizeTestComponent{}.x($width / 2 + 10_px), //.border("#FCAE1E"_color).border_width(8),
      HoverPressTest{}.x(200_px).y($height - 150_px).width(50_px).height(50_px),
    };
  }

  ON_EVENT(ASD, LOG::print{INFO}("asdf"))
};

template <std::size_t N>
struct str_t {
  constexpr str_t(const char (&s)[N]) {
    std::copy_n(s, N, val);
  }
  char val[N];
};

template <str_t Str>
struct something {
  static constexpr const char* str = "h"; // Str.val;
};

void sdfasdf() {
  using asdf                       = something<"hello">;
  static constexpr const char* sss = asdf::str;
  static constexpr const char* c   = __func__;
}


template <typename T, auto T::* Ptr>
struct field_ptr {
  using value_type = decltype(Ptr);
  using field_type = decltype(Ptr);
};

struct test_struct {
  int b;
  int a;
};

void fdasfdsa() {
  int test_struct::*           fasd{};
  static constexpr test_struct ts{};
  using f = field_ptr<test_struct, &test_struct::a>;

  using vt = f::value_type;
  using ft = f::field_type;

  using ttt = decltype(&test_struct::a);
  ttt tt    = &test_struct::a;
}


TEST("Debug panel") {
  // using namespace cydui::debug;

  LOG::INIT{}.filter({".*", "stdout"});

  std::string text{"TEXT: "};

  // auto win = cydui::CWindow::make<DebugPanel>()
  //            .size(777, 480)
  //            .title("Debug panel")
  //            .show();
  //
  // while (win->is_open());
  return 0;
}

TEST("Text Input") {
  LOG::INIT{}.log_everything();
  // LOG::INIT{}
  //   .filter({"include/.*", "stdout"})
  //   .filter({"test/.*", "stdout"})
  //   .filter()
  //   .path("async/.*")
  //   .levels({WARN, ERROR, FATAL})["stdout"];

  cydui::init();
  cydui::init_backend<cydui::backends::SDL3_backend>();

  PROF_CONFIG(cydui::backends::sdl3::p_window_events, true);
  PROF_CONFIG(cydui::layout::focus_state, true);
  PROF_CONFIG(cydui::layout::hover_state, true);
  PROF_CONFIG(cydui::layout::ui_style, true);
  PROF_CONFIG(cydui::layout::ui_tree, true);
  PROF_CONFIG(cydui::layout::ui_renderer, true);
  PROF_CONFIG(cydui::layout::ui_compositor, true);
  PROF_CONFIG(cydui::layout::ui_frame, true);
  PROF_CONFIG(cydui::layout::ui_updater, true);

  auto frame = cydui::make_frame<cydui::backends::SDL3_backend>("[TEST] Text input", 1080, 1080);
  frame->set_position(0, 0);

  std::string text{"TEXT: "};
  std::string text1{"TEXT: "};

  cydui::UI_options opts{};
  opts.attach_style(R"TSS(
HoverPressTest {
  background: #111177;
}

HoverPressTest:hover#pressed {
  background: #1155CC;
}

HoverPressTest:hover {
  background: #1111AA;
}

SizeTestComponent {
  background: #226622;
  border.left: #ff0000;
  border: #ff0000 #00ff00 #0000ff #fcae1e;
  border_width: 1 1 1 1;
  border_width.left: 1;
  border_width: {
    top: 5;
    left: 10;
    right: 15;
    bottom: 20;
  };
}
SizeTestComponent:hover {
  background: #337733;
}
stdui/input/text {
}
stdui/input/text:hover {
  background: #333333;
}
stdui/input/text:focus {
  background: #552222;
}
TestWithContext {
  background: #FF0000;
  val: 0.2;
}
TestWithContext#some-tag {
  background: #00FF00;
}
TestWithContext:hover {
  background: #0000FF;
}
TestWithContext#some-tag:hover {
  background: #00FFFF;
}

SelfSizedComponent {
  background: #552222;
}
SelfSizedComponent:hover {
  background: #aa2222;
}
AutoSizedComponent SelfSizedComponent {
  background: #115511;
}
AutoSizedComponent SelfSizedComponent:hover {
  background: #22aa22;
}
AutoSizedComponent {
  background: #111111;
}
AutoSizedComponent:hover {
  background: #666666;
}
)TSS");

  cydui::UI ui = cydui::make_ui(frame, OtherComponent{}, opts);

  // auto win1 = cydui::CWindow::make<TestComponent>({&text1})
  //             .size(640, 100)
  //             .title("[TEST] Text input")
  //             .show();

  while (true) {
    std::this_thread::sleep_for(100ms);
  }
  return 0;
}
