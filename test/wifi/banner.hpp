//
// Created by castle on 8/30/22.
//

#ifndef CYD_UI_BANNER_HPP
#define CYD_UI_BANNER_HPP

#include "../../include/cydui.hpp"
#include "../components/terminal_button.hpp"


class BannerState: public cydui::components::ComponentState {
public:
  containers::HBoxState hboxState;
};

class Banner: public cydui::components::Component {
public:
  NO_PROPS
  
  logging::logger log = {.name = "WIFI:Banner", .on = true};
  
  explicit Banner(
    BannerState* _state,
    Props props,
    const std::function<void(cydui::components::Component*)> &inner
  ): cydui::components::Component(_state) {
  }
  
  void on_redraw() override {
    auto* state = (BannerState*)this->state;
    auto* c     = new cydui::layout::color::Color("#FCAE1E");
    auto* c1    = new cydui::layout::color::Color("#0000FF");
    
    add(
      {
        (new containers::HBox(
          &(state->hboxState),
          10,
          [c, c1, state, this](cydui::components::Component* hbox) {
            hbox->add(
              {
                //new primitives::Rectangle(c, 0, 0, 32, 32, true),
                //new primitives::Text(
                //  c,
                //  new cydui::layout::fonts::Font {
                //    .name = "Fira Code Retina",
                //    .size = 14
                //  },
                //  0, 0, "[ WIFI ]"
                //),
                N(TerminalButton, ({
                  .cmd = "iwctl"
                })),
              }
            );
          }
        ))
        //->set_border_enable(true),
      }
    );
  }
};

#endif //CYD_UI_BANNER_HPP
