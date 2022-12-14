//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_PRIMITIVES_HPP
#define CYD_UI_PRIMITIVES_HPP


#include "../src/layout/color/colors.hpp"
#include "../src/layout/fonts/fonts.hpp"
#include "components.hpp"

namespace primitives {
  using namespace cydui;
  using namespace cydui::components;
  using namespace cydui::layout::color;
  using namespace cydui::events::layout;
  
  class Line: public Component {
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    Color* color;
    
    void on_redraw(CLayoutEvent* data) override;
  
  public:
    explicit Line(Color* color, int x1, int y1, int x2, int y2);
  };
  
  class Rectangle: public Component {
    int  x = 0, y = 0, w = 0, h = 0;
    bool filled;
    Color* color;
    
    void on_redraw(CLayoutEvent* data) override;
  
  public:
    explicit Rectangle(
        Color* color, int x, int y, int w, int h, bool filled = false
    );
  };
  
  class Arc: public Component {
    int  x = 0, y = 0, w = 0, h = 0;
    int  a0, a1;
    bool filled;
    Color* color;
    
    void on_redraw(CLayoutEvent* data) override;
  
  public:
    explicit Arc(
        Color* color,
        int x,
        int y,
        int w,
        int h,
        int a0,
        int a1,
        bool filled = false
    );
  };
  
  class Circle: public Component {
    int  x = 0, y = 0, w = 0, h = 0;
    bool filled;
    Color* color;
    
    void on_redraw(CLayoutEvent* data) override;
  
  public:
    explicit Circle(
        Color* color, int x, int y, int w, int h, bool filled = false
    );
  };
  
  class Text: public Component {
    
    
    int x = 0, y = 0;
    Color              * color;
    layout::fonts::Font* font;
    std::string text;
    
    void on_redraw(CLayoutEvent* data) override;
  
  public:
    Text(
        Color* color, layout::fonts::Font* font, int x, int y, std::string text
    );
  };
  
}// namespace primitives


#endif//CYD_UI_PRIMITIVES_HPP
