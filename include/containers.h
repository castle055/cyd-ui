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
  
  class VBox: public Component { ;
    
    void on_redraw(CLayoutEvent* ev) override;
  
  public:
    int spacing;
    
    explicit VBox(
        cydui::components::ComponentState* state,
        int spacing,
        std::function<void(cydui::components::Component*)> inner
    );
  };
  
  class HBox: public Component { ;
    
    void on_redraw(CLayoutEvent* ev) override;
  
  public:
    int spacing;
    
    explicit HBox(
        cydui::components::ComponentState* state,
        int spacing,
        std::function<void(cydui::components::Component*)> inner
    );
  };
  
}// namespace containers


#endif//CYD_UI_CONTAINERS_H
