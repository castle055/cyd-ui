//
// Created by castle on 8/30/22.
//

#ifndef CYD_UI_WIFI_H
#define CYD_UI_WIFI_H

#include "../../include/cydui.h"
#include "../../src/logging/logging.h"
#include "banner.h"
#include "../components/button.h"

static const logging::logger log = {.name = "MAIN", .on = true};

class TestState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  cydui::components::ComponentState vboxState;
  cydui::components::ComponentState hboxState;
  BannerState                       banner_state;
  ButtonState                       button_state;
  ButtonState                       button2_state;
  
  bool button2 = false;
};

class TestComponent: public cydui::components::Component {
public:
  explicit TestComponent(TestState* _state)
      : cydui::components::Component(
      _state, std::vector<cydui::components::Component*>()) {
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto* state = (TestState*)this->state;
    auto* c     = new cydui::layout::color::Color("#FCAE1E");
    auto* c1    = new cydui::layout::color::Color("#0000FF");
    
    add(
        {
            (new containers::HBox(
                &(state->hboxState),
                false,
                10,
                {
                    (new containers::VBox(
                        &(state->vboxState),
                        false,
                        8,
                        {
                            (new Banner(&(state->banner_state))),
                            !state->button2?
                            (new Button(
                                &(state->button_state), "TEST 1", [state]() {
                                  state->button2 = true;
                                  state->dirty();
                                  log.error("CLICK 1");
                                }
                            )) :
                            (new Button(
                                &(state->button2_state), "TEST 2", [state]() {
                                  state->button2 = false;
                                  state->dirty();
                                  log.error("CLICK 2");
                                }
                            )),
                        }
                    ))
                        ->set_border_enable(true)
                        ->set_pref_size(state->vboxState.geom.content_w(), state->hboxState.geom.content_h() - 10),
                    
                    new primitives::Rectangle(c, 0, 0, 32, 32, true),
                }
            ))
                //->set_pos(this, 15, 5)
                ->set_padding(5, 0, 0, 15)
                ->set_margin(3, 3, 3, 3)
                ->set_border_enable(true)
                ->set_pref_size(state->geom.content_w(), state->geom.content_h()),
        }
    );
  }
};

#endif //CYD_UI_WIFI_H
