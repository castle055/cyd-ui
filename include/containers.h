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
  
  
  class VBoxState: public ComponentState {
  public:
    IntProperty spacing;
    IntProperty offset;
  };
  
  class VBox: public Component { ;
    
    void on_redraw(CLayoutEvent* ev) override;
  
  public:
    explicit VBox(
        containers::VBoxState* _state,
        int spacing,
        std::function<void(cydui::components::Component*)> inner
    );
  };
  
  class HBoxState: public ComponentState {
  public:
    IntProperty spacing;
    IntProperty offset;
  };
  
  class HBox: public Component { ;
    
    void on_redraw(CLayoutEvent* ev) override;
  
  public:
    
    explicit HBox(
        HBoxState* _state,
        int spacing,
        std::function<void(cydui::components::Component*)> inner
    );
  };
  
}// namespace containers


#endif//CYD_UI_CONTAINERS_H
