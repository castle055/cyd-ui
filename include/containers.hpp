//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_CONTAINERS_HPP
#define CYD_UI_CONTAINERS_HPP

#include "cydui.hpp"

STATE(VBox) { };

COMPONENT(VBox) {
  PROPS({
    dimensions::dimensional_relation_t spacing = 0;
    int offset = 0;
  })
  INIT(VBox) {
  }
  REDRAW {
    Component* prev = nullptr;
    for (auto &ch: children) {
      ch->dim->y = prev == nullptr
        ? props.offset
        : prev->dim->y + prev->dim->h + props.spacing;
      prev = ch;
    }
  }
};

STATE(HBox) { };

COMPONENT(HBox) {
  PROPS({
    dimensions::dimensional_relation_t spacing = 0;
    int offset = 0;
  })
  INIT(HBox) {
  }
  REDRAW {
    Component* prev = nullptr;
    for (auto &ch: children) {
      ch->dim->x = prev == nullptr
        ? props.offset
        : prev->dim->x + prev->dim->w + props.spacing;
      prev = ch;
    }
  }
};


#endif//CYD_UI_CONTAINERS_HPP
