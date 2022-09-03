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
  cydui::components::ComponentState                  vboxState;
  cydui::components::ComponentState                  hboxState;
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
                  (state->geom.content_w()),
                  state->inner
              ))
                  ->set_pref_size(state->geom.content_w(), state->geom.content_h()),
          }
      );
    } else {
      add(
          {
              (new containers::HBox(
                  &(state->hboxState),
                  0,
                  [state](cydui::components::Component* _hbox) {
                    auto* hbox = (containers::HBox*)_hbox;
                    
                    state->inner(hbox);
                    
                    int             c     = -1;
                    int             sum_w = 0;
                    for (const auto &item: hbox->children) {
                      c++;
                      sum_w += item->state->geom.abs_w();
                    }
                    
                    hbox->spacing =
                        (state->geom.content_w() - sum_w) / c - 1;
                  }
              ))
                  ->set_border_enable(true)
                  ->set_pref_size(state->geom.content_w(), state->geom.content_h())
          }
      );
    }
  }
};

#endif //CYD_UI_FLEXBOX_H
