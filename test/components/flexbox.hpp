//
// Created by castle on 9/1/22.
//

#ifndef CYD_UI_FLEXBOX_HPP
#define CYD_UI_FLEXBOX_HPP

#include <utility>

#include "../../include/cydui.hpp"
#include "../../src/logging/logging.hpp"
#include "../components/button.hpp"

class FlexBoxState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  containers::VBoxState                              vboxState;
  containers::HBoxState                              hboxState;
  std::function<void(cydui::components::Component*)> inner = { };
};

class FlexBox: public cydui::components::Component {
public:
  PROPS({
    bool vertical = false;
  })
  
  explicit FlexBox(
    FlexBoxState* _state,
    Props props,
    std::function<void(cydui::components::Component*)> inner
  )
    : cydui::components::Component(_state) {
    this->props   = props;
    _state->inner = std::move(inner);
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto* state = (FlexBoxState*)this->state;
    
    if (props.vertical) {
      add(
        {
          (new containers::VBox(
            &(state->vboxState),
            0,
            [state](cydui::components::Component* _vbox) {
              auto* vbox       = (containers::VBox*)_vbox;
              auto* vbox_state = (containers::VBoxState*)_vbox->state;
              
              state->inner(vbox);
              
              std::vector<Property*> deps = {vbox_state->geom.content_h().unwrap()};
              for (auto              &item: vbox->children) {
                deps.push_back(item->state->geom.abs_h().unwrap());
              }
              
              vbox_state->spacing.set_binding(
                [vbox]() {
                  int             c     = -1;
                  int             sum_h = 0;
                  for (const auto &item: vbox->children) {
                    c++;
                    sum_h += item->state->geom.abs_h().compute();
                  }
                  
                  int spc = 0;
                  if (c > 0)
                    spc = (vbox->state->geom.content_h().compute() - sum_h) / c - 1;
                  if (spc < 0)
                    spc = 0;
                  return spc;
                }, deps
              );
              
              //if (state->geom.custom_width) {
              //  vbox->set_width(state->geom.content_w().compute());
              //}
              //if (state->geom.custom_height) {
              //  vbox->set_height(state->geom.content_h().compute());
              //}
            }
          ))
          //->set_border_enable(true)
        }
      );
    } else {
      add(
        {
          (new containers::HBox(
            &(state->hboxState),
            0,
            [state](cydui::components::Component* _hbox) {
              auto* hbox       = (containers::HBox*)_hbox;
              auto* hbox_state = (containers::HBoxState*)_hbox->state;
              
              state->inner(hbox);
              
              std::vector<Property*> deps = {hbox_state->geom.content_w().unwrap()};
              for (auto              &item: hbox->children) {
                deps.push_back(item->state->geom.abs_w().unwrap());
              }
              
              hbox_state->spacing.set_binding(
                [hbox]() {
                  int             c     = -1;
                  int             sum_w = 0;
                  for (const auto &item: hbox->children) {
                    c++;
                    sum_w += item->state->geom.abs_w().compute();
                  }
                  
                  int spc = 0;
                  if (c > 0)
                    spc = (hbox->state->geom.content_w().compute() - sum_w) / c - 1;
                  if (spc < 0)
                    spc = 0;
                  return spc;
                }, deps
              );
              
              //if (state->geom.custom_width) {
              //  hbox->set_width(state->geom.content_w().compute());
              //}
              //if (state->geom.custom_height) {
              //  hbox->set_height(state->geom.content_h().compute());
              //}
            }
          ))
          //->set_border_enable(true)
        }
      );
    }
  }
};

#endif //CYD_UI_FLEXBOX_HPP
