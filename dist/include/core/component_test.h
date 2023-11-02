//
// Created by castle on 10/20/23.
//

#ifndef CYD_UI_COMPONENT_TEST_H
#define CYD_UI_COMPONENT_TEST_H

#include "component.h"

#define SET_COMPONENT_TEMPLATE <typename T>
#define SET_COMPONENT_TEMPLATE_SHORT <T>
STATE_TEMPLATE(SomeComponent)
{ };
COMPONENT_TEMPLATE(SomeComponent, {
  T a;
}) {
  ON_REDRAW {
    return {};
  }
};

STATE_TEMPLATE(Transparent)
{ };
COMPONENT_TEMPLATE(Transparent, {
  typename T::props_t props {};
}) {
  ON_REDRAW {
    return {
      T {this->props->props},
    };
  }
};

EVENT(TestEvent, {
  std::string hello_world;
})

STATE(OtherComponent) {
  std::vector<long> some_ints1 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
};
COMPONENT(OtherComponent, {
  bool some_prop;
}) {
  ON_REDRAW {
    return {
      SomeComponent<int> {{3}}
        .x(10)
        .y(100),
      with {state->some_ints1}
        .map_to(
        [this](long &it) -> map_to_result_t {
          return {SomeComponent<long> {{it}}};
        }),
      with {props->some_prop}
        .then({SomeComponent<long> {{1}}})
        .or_else({SomeComponent<double> {{1.0}}}),
    };
  }
  
  ON_CUSTOM_EVENTS({
    ON_EVENT(TestEvent, {
      ev->hello_world;
    }),
  })
};


struct init_t {
  template<typename Func>
  explicit init_t(Func &&fun) {
    fun();
  }
};
#define init(...) init_t init { [this]() __VA_ARGS__ }

struct asdfhalsd {
  int a = 0;
  int b = 1;
  init({
    a;
    b;
  });
};

#endif //CYD_UI_COMPONENT_TEST_H