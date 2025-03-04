//
// Created by castle on 8/15/24.
//

#include "cyd_ui/components/component_macros.h"

#include <tracy/Tracy.hpp>
#include "common.h"

import reflect;
import fabric.logging;
import cydui.std;
import cydui.animations;

using namespace stdui;
using namespace std::chrono_literals;

void setup() {
}

struct model_context {
  color::Color color {"#FF0000"_color};
  std::string* text{nullptr};
};

struct styletype {
  color::Color bg{"#ff0000"_color};
};


COMPONENT(
  View, //
  { std::string* text; };
  SIGNAL(pressed);
  STYLE EXTENDS(styletype) {
    cydui::dimension_t some_dim{0_px};
    double             val{0};
  };
) {
  use_context<const model_context> model_ctx;
  CHILDREN {
    return {//
    };
  }

  ON_BUTTON_RELEASE {
    component.pressed.emit();
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

COMPONENT(Presenter, { std::string* text; }) {
  cydui::provide_context<model_context> model =
    bind{&props_t::text}.transform([](std::string* const & s) { return s; });
  CHILDREN {
    return {
      model > with_context {
        input::text{{props.text}}.height(30_px).width($width / 2),
        View {{props.text}}.x(50_px).y(50_px)
                .width(200_px).height(200_px)
                .id("animation_target")
                .on_pressed([&] {
                  auto c = component.find_child("animation_target").value();
                })
                .border("#00FF00"_color)
                .border_width(2)["some-tag"],
      },
    };
  }
};

COMPONENT(OtherComponent, {}) {
  std::string text1 = "Well, hello there!";
  std::string text2 = "Well, hello there!";
  CHILDREN {
    return {
      Presenter{{&text1}}.width($width).height($height / 2 - 1),
      Presenter{{&text2}}.width($width).height($height / 2 - 1).y($height / 2 + 1),
    };
  }

  FRAGMENT {
    fragment
      .draw<vg::rectangle>() //
      .x(2 * $width / 3)
      .y($height / 3)
      .w($height / 4)
      .h($height / 4)
      .stroke("#FCAE1E"_color)
      .stroke_width(3)
      .r(20);
  }
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
  static constexpr const char* str = Str.val;
};

void sdfasdf() {
  using asdf = something<"hello">;
  static constexpr const char* sss = asdf::str;
}



template <typename T, auto T::*Ptr>
struct field_ptr {
  using value_type = decltype(Ptr);
  using field_type = decltype(Ptr);
};

struct test_struct {
  int b;
  int a;
};

void fdasfdsa() {
  int test_struct::* fasd{};
  static constexpr test_struct ts{};
  using f = field_ptr<test_struct, &test_struct::a>;

  using vt = f::value_type;
  using ft = f::field_type;

  using ttt = decltype(&test_struct::a);
  ttt tt = &test_struct::a;
}

TEST("Data Binding") {
  LOG::INIT { }.filter({".*", "stdout"});

  std::string text {"TEXT: "};
  std::string text1 {"TEXT: "};

  auto win = cydui::CWindow::make<OtherComponent>()
               .size(640, 100)
               .title("[TEST] Text input")
               .style(R"TSS(
stdui/input/text {
}
stdui/input/text:hover {
  background: #333333;
}
stdui/input/text:focus {
  background: #552222;
}
View {
  background: #FF0000;
  val: 0.2;
}
View#some-tag {
  background: #00FF00;
}
View:hover {
  background: #0000FF;
}
View#some-tag:hover {
  background: #00FFFF;
}
)TSS")
               .show();

  while (win->is_open()) {
    std::this_thread::sleep_for(100ms);
  }
  return 0;
}
