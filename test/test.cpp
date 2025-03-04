//
// Created by castle on 8/15/24.
//

#include "cyd_ui/components/component_macros.h"

#include <tracy/Tracy.hpp>
#include "common.h"

import fabric.logging;
import cydui.std;
import cydui.animations;

using namespace stdui;
using namespace std::chrono_literals;

void setup() {
}


struct test_context {
  color::Color color {"#FF0000"_color};
};

struct styletype {
  color::Color bg{"#ff0000"_color};
};
COMPONENT(
  TestWithContext, //
  { std::string* text; };
  SIGNAL(pressed);
  STYLE EXTENDS(styletype) {
    cydui::dimension_t some_dim{0_px};
    double               val{0};
  };
) {
  use_context<test_context> test_ctx;
  CHILDREN {
    // style_t sasd{{.background = vg::paint::type::make<vg::paint::solid>("#ffffff"_color)}};
    // $component.background(test_ctx->color);
    // $component.background(style.background);

    // style.some_dim

    return {};
  }

  ON_BUTTON_RELEASE {
    // test_ctx->color = "#00FF00"_color;
    // test_ctx.notify();
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


cydui::animation anim{
  {                                                                   //
   cydui::keyframe::make(1.0, TestWithContext::style_t{.val = 1.0}) //
     .interp("val", cydui::interp::bezier{{1.0, 0.2}, {0.4, 0.0}})
  },
  cydui::animation_opts{} //
    .easing(cydui::easing::linear)
    .duration(3000ms),
};

COMPONENT(TestComponent, { std::string* text; }) {
  use_context<test_context> dependency_test_ctx{};
  cydui::provide_context<test_context> test_ctx{};
  CHILDREN {
    return {
      test_ctx > with_context {
        TestWithContext { }.x(50_px).y(50_px)
                           .width(200_px).height(200_px)
                           .id("animation_target")
                           .on_pressed([&] {
                             auto c = component.find_child("animation_target").value();
                             cydui::animate(c, anim);
                           })
                           .border("#00FF00"_color)
                           .border_width(2)["some-tag"],
        input::text {{props.text}}.height(30_px).width($width / 2).on_enter([&] {
          std::cout << "ENTER!" << std::endl;
        }),
      },
    };
  }
};

COMPONENT(OtherComponent, {}) {
  std::string text1 = "Well, hello there!";
  std::string text2 = "Well, hello there!";
  CHILDREN {
    return {
      TestComponent{{&text1}}.width($width).height($height / 2 - 1),
      TestComponent{{&text2}}.width($width).height($height / 2 - 1).y($height / 2 + 1),
    };
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
  static constexpr const char* str = "h";//Str.val;
};

void sdfasdf() {
  using asdf = something<"hello">;
  static constexpr const char* sss = asdf::str;
  static constexpr const char* c = __func__;
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



TEST("Debug panel") {
  // using namespace cydui::debug;

  LOG::INIT { }.filter({".*", "stdout"});

  std::string text {"TEXT: "};

  // auto win = cydui::CWindow::make<DebugPanel>()
  //            .size(777, 480)
  //            .title("Debug panel")
  //            .show();
  //
  // while (win->is_open());
  return 0;
}

TEST("Text Input") {
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
)TSS")
             .show();

  // auto win1 = cydui::CWindow::make<TestComponent>({&text1})
  //             .size(640, 100)
  //             .title("[TEST] Text input")
  //             .show();

  while (win->is_open()) {
    std::this_thread::sleep_for(100ms);
  }
  return 0;
}

//=============================================================================

// struct TestComp;
// struct EventHandlerTestComp;

// struct ImplTestComp: public cydui::components::component_t<EventHandlerTestComp, ImplTestComp> {
//   using sptr = std::shared_ptr<ImplTestComp>;

//   static constexpr const char* NAME = "TestComp";
//   std::string name() override { return std::string {NAME}; }
//   using event_handler_t = EventHandlerTestComp;
//   struct init;

//   struct props_t {
//     std::string &text;
//   };

// public:
//   props_t props;
//   using state_t = std::conditional<is_type_complete_v<struct init>, init, cydui::components::component_state_t>::type;

//   template<typename P = props_t>
//   explicit ImplTestComp(
//     std::enable_if_t<std::is_default_constructible_v<P>, props_t> props = { }
//   ) : cydui::components::component_t<EventHandlerTestComp, ImplTestComp>(), props(std::move(props)) {
//   }

//   explicit ImplTestComp(props_t props) : cydui::components::component_t<EventHandlerTestComp, ImplTestComp>(),
//                                      props(std::move(props)) {
//   }

//   ~ImplTestComp() override = default;

//   void* get_props() override { return (void*)&(this->props); }
//   friend struct EventHandlerTestComp;
//   friend struct EventHandlerDataTestComp;
// };

// struct TestComp {
//   using props_type = ImplTestComp::props_t;
//   using state_type = std::shared_ptr<ImplTestComp::state_t>;

//   TestComp() = default;
//   template <typename ...Props>

//   explicit TestComp(Props&&... props) {

//   }

//   TestComp(const props_type& props) {

//   }
//   TestComp(props_type&& props) {

//   }

//   TestComp(const TestComp& rhl) {

//   }
//   TestComp(TestComp&& rhl) noexcept {

//   }
//   TestComp& operator=(const TestComp& rhl) {

//   }
//   TestComp& operator=(TestComp&& rhl) noexcept {

//   }
// public:
//   component_state_ref make_state() const {
//     return impl->create_state_instance();
//   }
// private:
//   ImplTestComp::sptr impl;
// };

// struct EventHandlerDataTestComp: public cydui::components::event_handler_t {
//   EventHandlerDataTestComp(
//     event_handler_t &parent_,
//     const std::vector<std::shared_ptr<component_base_t>> &$children_,
//     cyd::fabric::async::async_bus_t &window_,
//     const TestComp::state_type &state_,
//     const TestComp::props_type &props_,
//     attrs_component<ImplTestComp> &attrs_
//   ) : event_handler_t(parent_, $children_),
//       window(window_),
//       state(state_),
//       props(props_),
//       attrs(attrs_) {
//   }

//   cyd::fabric::async::async_bus_t &window;
//   TestComp::state_type state;
//   const TestComp::props_type &props;
//   attrs_component<ImplTestComp> &attrs;
//   ImplTestComp* component_instance() const { return dynamic_cast<ImplTestComp*>(this->state->component_instance.value()); }
// };

// struct EventHandlerTestComp final: public EventHandlerDataTestComp {
//   std::vector<cydui::components::component_holder_t> on_redraw() override {
//     return {
//       input::text {{props.text}}.h($ch() / 2 - 1).w($cw()).on_enter([&] {
//         std::cout << "ENTER!" << std::endl;
//       }),
//     };
//   }

//   cyd::fabric::async::listener_t* listener = window.on_event<RedrawEvent>([&](const auto &it) {
//     this->on_event_RedrawEvent(it);
//   });
//   void on_event_RedrawEvent(const RedrawEvent& e) {

//   }
// };

// template <typename T>
// struct comparable {
//   bool operator==(const comparable &rhs) const = default;
// };

// struct fdafas: comparable<fdafas> {
//   // std::function<void()> fdsa {[]{}};


// };


// template <typename T>
// void testt(T t) {
//   auto [v1] = t;
// }

// void fffffffffffffffff() {
//   fdafas f{};

//   testt(f);
// }

// // bool operator==(const fdafas& lhs, const fdafas& rhs) {
//   // return lhs.fdsa == rhs.fdsa;
// // }

// void asdfasdfasdfasdasdf() {
//   std::string text{};
//   TestComp asdf {{text}};
//   TestComp asdfa {text};

//   TestComp::state_type state = asdf.make_state();

//   fdafas f, ff;
//   if (f == ff) {

//   }
// }
