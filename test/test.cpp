//
// Created by castle on 8/15/24.
//

#include "cyd_ui/components/component_macros.h"

#include "common.h"
import cydui.std;

using namespace stdui;

void setup() {
}

COMPONENT(TestComponent, {
  std::string& text;
}) {
  ON_REDRAW {
    return {
      input::text {{props->text}}.h($ch() / 2 - 1).w($cw()).on_enter([&] {
        std::cout << "ENTER!" << std::endl;
      }),
      input::text {{props->text}}.y($ch() / 2 + 1).h($ch() / 2 - 1).w($cw()),
    };
  }
};

TEST("Text Input") {
  std::string text{"TEXT: "};
  auto win = cyd::ui::window::create(
    cyd::ui::layout::create(TestComponent {{text}}),
    "text input", "scratch", 0, 0, 640, 100
  );
  while (win->is_open());
  return 0;
}

