//
// Created by castle on 8/22/22.
//

#include "../../../include/containers.hpp"

#include <utility>
#include "../../../include/primitives.hpp"
#include "../../../include/logging.hpp"

logging::logger cont_log = {.name = "CONT"};

containers::VBox::VBox(
  containers::VBoxState* _state,
  int spacing,
  std::function<void(cydui::components::Component*)> inner
)
  : Component(_state, inner) {
  _state->spacing = spacing;
  _state->spacing.bind(_state);
  _state->offset = 1;
}

void containers::VBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  auto* state = (VBoxState*)this->state;
  
  int       cur_h = state->offset.val();
  for (auto &ch: children) {
    ch->set_pos(this, 1, cur_h);
    cur_h += (state->spacing.val() + ch->state->geom.abs_h().compute());
  }
}

containers::HBox::HBox(
  containers::HBoxState* _state,
  int spacing,
  std::function<void(cydui::components::Component*)> inner
)
  : Component(_state, inner) {
  _state->spacing = spacing;
  _state->spacing.bind(_state);
  _state->offset = 1;
}

void containers::HBox::on_redraw(cydui::events::layout::CLayoutEvent* ev) {
  auto* state = (HBoxState*)this->state;
  
  int       cur_w = state->offset.val();
  for (auto &ch: children) {
    ch->set_pos(this, cur_w, 1);
    cur_w += (state->spacing.val() + ch->state->geom.abs_w().compute());
  }
}
