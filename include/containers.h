//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_CONTAINERS_H
#define CYD_UI_CONTAINERS_H


#include "../src/layout/color/colors.h"
#include "components.h"

namespace containers {
  using namespace cydui;
  using namespace cydui::components;
  using namespace cydui::layout::color;
  using namespace cydui::events::layout;

  class VBox: public Component {
    ;
    void on_redraw(CLayoutEvent* ev) override;
    int spacing;
    bool border;

  public:
    explicit VBox(ComponentState* state,
        bool border,
        int spacing,
        std::vector<cydui::components::Component*> children);
  };

  class HBox: public Component {
    ;
    void on_redraw(CLayoutEvent* ev) override;
    int spacing;
    bool border;

  public:
    explicit HBox(ComponentState* state,
        bool border,
        int spacing,
        std::vector<cydui::components::Component*> children);
  };

}// namespace containers


#endif//CYD_UI_CONTAINERS_H
