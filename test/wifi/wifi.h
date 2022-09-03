//
// Created by castle on 8/30/22.
//

#ifndef CYD_UI_WIFI_H
#define CYD_UI_WIFI_H

#include "../../include/cydui.h"
#include "../../src/logging/logging.h"
#include "banner.h"
#include "../components/button.h"
#include "../components/flexbox.h"
#include "../../src/events/properties/properties.h"

static const logging::logger log = {.name = "MAIN", .on = true};

class TestState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  cydui::components::ComponentState vboxState;
  FlexBoxState                      flex_box_state;
  BannerState                       banner_state;
  ButtonState                       button_state;
  ButtonState                       button2_state;
  
  bool button2 = false;
  
  IntProperty prop1;
  
  explicit TestState(): cydui::components::ComponentState() {
    prop1.bindComponentRender(this);
  }
};

class TestComponent: public cydui::components::Component {
public:
  explicit TestComponent(TestState* state)
      : cydui::components::Component(state) {
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto* state = (TestState*)this->state;
    
    //int val = state->prop2;
    
    auto* prop2 = state->prop1 * state->prop1 + 1 / 2;
    prop2 + 1;
    add(
        {
            (new FlexBox(
                &(state->flex_box_state),
                false,
                [state](cydui::components::Component* fbox) {
                  auto* c  = new cydui::layout::color::Color("#FCAE1E");
                  auto* c1 = new cydui::layout::color::Color("#0000FF");
                  fbox->add(
                      {
                          (new containers::VBox(
                              &(state->vboxState),
                              8,
                              [state](cydui::components::Component* vbox) {
                                vbox->add(
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
                                );
                              }
                          )),
                          new primitives::Rectangle(c, 0, 0, 32, 32, true),
                      }
                  );
                }
            ))
                ->set_padding(5, 5, 5, 5)
                ->set_margin(5, 5, 5, 5)
                ->set_border_enable(true)
                ->set_pref_size(state->geom.content_w(), state->geom.content_h()),
        }
    );
  }
};

#endif //CYD_UI_WIFI_H
