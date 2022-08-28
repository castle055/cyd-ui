//
// Created by castle on 8/21/22.
//

#include "../include/cydui.h"
#include "../src/logging/logging.h"

const logging::logger log = { .name = "MAIN"};

class TestState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  cydui::components::ComponentState vboxState;
  cydui::components::ComponentState hboxState;
};
class TestComponent: public cydui::components::Component {
public:
  explicit TestComponent(TestState* _state)
      : cydui::components::Component(_state, std::vector<cydui::components::Component*>()) { }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto state = (TestState*)this->state;
    auto* c = new cydui::layout::color::Color("#FCAE1E");
    auto* c1 = new cydui::layout::color::Color("#0000FF");
    
    add({ new primitives::Rectangle(c, 5, 5, state->w-10, state->h-10, false) });
  
    add({
            (new containers::VBox(&(state->vboxState), true, (state->h-62)/4, {
                new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                new primitives::Rectangle(c, 5, 0, 10, 10, true),
                new primitives::Rectangle(c1, 10, 0, 10, 10, true),
                (new containers::HBox(&(state->hboxState), true, (state->w-50)/3, {
                    new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                    new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                    new primitives::Rectangle(c, 0, 0, 10, 10, true),
                    new primitives::Rectangle(c1, 0, 0, 10, 10, true),
                })),
            }))->set_pos(5, 5),
        });
  }
};

int main() {
  // Create layout tree
  
  // Instantiate window with layout
  auto* test_state = new TestState();
  auto layout = new cydui::layout::Layout(new TestComponent(test_state));
  cydui::window::CWindow* win = cydui::window::create(layout, "test_window", "scratch");
  cydui::graphics::window_t* w = win->win_ref;
  
  // Interact with window through window pointer
  
  while(1);
  return 0;
}
