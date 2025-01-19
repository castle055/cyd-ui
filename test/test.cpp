//
// Created by castle on 8/15/24.
//

#include "cyd_ui/components/component_macros.h"

#include "common.h"
import cydui.std;

using namespace stdui;

void setup() {
}

struct COMPONENT(TestComponent, {
  std::string* text;
}) {
  ON_REDRAW {
    return {
      input::text {{props.text}}.height($height / 2 - 1).width($width).on_enter([&] {
        std::cout << "ENTER!" << std::endl;
      }),
      input::text {{props.text}}.y($height / 2 + 1).height($height / 2 - 1).width($width),
    };
  }
};

TEST("Text Input") {
  std::string text{"TEXT: "};
  auto win = cyd::ui::CWindow::make<TestComponent>({&text}).size(640, 100).title("Text Input").show();
  while (win->is_open());
  return 0;
}

