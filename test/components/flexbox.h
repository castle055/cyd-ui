//
// Created by castle on 9/1/22.
//

#ifndef CYD_UI_FLEXBOX_H
#define CYD_UI_FLEXBOX_H

#include <utility>

#include "../../include/cydui.h"
#include "../../src/logging/logging.h"
#include "../components/button.h"

class FlexBoxState: public cydui::components::ComponentState {
public:
  //=== CHILDREN STATE DECLARATIONS
  containers::VBoxState                              vboxState;
  containers::HBoxState                              hboxState;
  std::function<void(cydui::components::Component*)> inner = { };
};

class FlexBox: public cydui::components::Component {
public:
  //std::unordered_set<cydui::components::Component*>         inner_children;
  bool vertical = false;
  
  explicit FlexBox(
      FlexBoxState* _state,
      bool vertical,
      std::function<void(cydui::components::Component*)> inner
  )
      : cydui::components::Component(_state) {
    this->vertical = vertical;
    _state->inner  = std::move(inner);
    //for (const auto &item: children) {
    //inner_children.insert(item);
    //}
  }
  
  void on_redraw(cydui::events::layout::CLayoutEvent* ev) override {
    auto* state = (FlexBoxState*)this->state;
    
    if (vertical) {
      add(
          {
              (new containers::VBox(
                  &(state->vboxState),
                  10,
                  [state](cydui::components::Component* _vbox) {
                    auto* vbox       = (containers::VBox*)_vbox;
                    auto* vbox_state = (containers::VBoxState*)_vbox->state;
                    
                    state->inner(vbox);
                    
                    int             c     = -1;
                    int             sum_w = 0;
                    for (const auto &item: vbox->children) {
                      c++;
                      sum_w += item->state->geom.abs_w().compute();
                    }
                    
                    vbox_state->spacing =
                        (state->geom.content_w() - sum_w) / c - 1;
                    vbox_state->spacing.bind(state);
                  }
              ))
                  //->set_border_enable(true)
                  ->set_size(state->geom.content_w(), state->geom.content_h())
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
                    
                    int             c     = -1;
                    int             sum_w = 0;
                    for (const auto &item: hbox->children) {
                      c++;
                      sum_w += item->state->geom.abs_w().compute();
                    }
                    
                    hbox_state->spacing =
                        (state->geom.content_w() - sum_w) / c - 1;
                    hbox_state->spacing.bind(state);
                  }
              ))
                  //->set_border_enable(true)
                  ->set_size(state->geom.content_w(), state->geom.content_h())
          }
      );
    }
  }
};

#endif //CYD_UI_FLEXBOX_H
