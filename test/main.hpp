//
// Created by castle on 8/28/22.
//

#ifndef CYD_UI_MAIN_HPP
#define CYD_UI_MAIN_HPP

#include "../include/cydui.hpp"
#include "../src/logging/logging.hpp"

const logging::logger log = {.name = "MAIN"};

class TestState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  cydui::components::ComponentState vboxState;
  cydui::components::ComponentState hboxState;
};

class TestComponent: public cydui::components::Component {
public:
  explicit TestComponent(TestState* _state)
      : cydui::components::Component(
      _state, std::vector<cydui::components::Component*>()) {
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (TestState*)this->state;
    auto* c  = new cydui::layout::color::Color("#FCAE1E");
    auto* c1 = new cydui::layout::color::Color("#0000FF");
    
    add(
        {
            new primitives::Rectangle(
                c, 5, 5, state->w - 10, state->h - 10, false
            )
        }
    );
    
    add(
        {
            (new containers::VBox(
                &(state->vboxState),
                true,
                (state->h - 62) / 4,
                {
                    new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                    new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                    new primitives::Rectangle(c, 5, 0, 10, 10, true),
                    new primitives::Rectangle(c1, 10, 0, 10, 10, true),
                    (new containers::HBox(
                        &(state->hboxState),
                        true,
                        (state->w - 50) / 3,
                        {
                            new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                            new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                            new primitives::Rectangle(c, 0, 0, 10, 10, true),
                            new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                        }
                    )),
                }
            ))
                ->set_pos(5, 5),
        }
    );
    
    add(
        {
            new primitives::Text(
                c,
                new cydui::layout::fonts::Font {
                    .name = "Fira Code Retina",
                    .size = 14
                },
                20, 20, "Hello World!"
            ),
        }
    );
  }
};


#endif //CYD_UI_MAIN_HPP
