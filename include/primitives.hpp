//
// Created by castle on 8/22/22.
//

#ifndef CYD_UI_PRIMITIVES_HPP
#define CYD_UI_PRIMITIVES_HPP


#include "colors.hpp"
#include "fonts.hpp"
#include "window_types.hpp"
#include "logging.hpp"
#include "components.hpp"

namespace primitives {
  using namespace cydui;
  using namespace cydui::components;
  using namespace cydui::layout::color;
  
  
  STATE(Line) };
  
  COMPONENT(Line)
    PROPS({
      Color* color;
    })
    
    INIT(Line) DISABLE_LOG }
    
    RENDER(win) {
      graphics::drw_line(
        win->win_ref,
        props.color,
        state->geom.content_x().val(),
        state->geom.content_y().val(),
        (state->geom.content_x() + state->geom.w).val(),
        (state->geom.content_y() + state->geom.h).val()
      );
    }
  };
  
  
  STATE(Rectangle) };
  
  COMPONENT(Rectangle)
    PROPS({
      Color* color;
      bool filled = false;
    })
    
    INIT(Rectangle) DISABLE_LOG }
    
    RENDER(win) {
      graphics::drw_rect(
        win->win_ref,
        props.color,
        state->geom.content_x().val(),
        state->geom.content_y().val(),
        state->geom.w.val(),
        state->geom.h.val(),
        props.filled
      );
    }
  };
  
  
  STATE(Arc) };
  
  COMPONENT(Arc)
    PROPS({
      Color* color;
      bool filled = false;
      int  a0     = 0;
      int  a1     = 180;
    })
    
    INIT(Arc) DISABLE_LOG }
    
    RENDER(win) {
      graphics::drw_arc(
        win->win_ref,
        props.color,
        state->geom.content_x().val(),
        state->geom.content_y().val(),
        state->geom.w.val(),
        state->geom.h.val(),
        props.a0, props.a1, props.filled
      );
    }
  };
  
  
  STATE(Circle) };
  
  COMPONENT(Circle)
    PROPS({
      Color* color;
      bool filled;
    })
    
    INIT(Circle) DISABLE_LOG }
    
    RENDER(win) {
      graphics::drw_arc(
        win->win_ref, props.color,
        state->geom.content_x().val(),
        state->geom.content_y().val(),
        state->geom.w.val(),
        state->geom.h.val(),
        0, 360, props.filled
      );
    }
  };
  
  
  STATE(Text) };
  
  COMPONENT(Text)
    PROPS({
      Color              * color;
      layout::fonts::Font* font;
      std::string text;
    })
    
    INIT(Text) DISABLE_LOG
      state->geom.w             = 125;
      state->geom.h             = this->props.font->size;
      state->geom.custom_width  = true;
      state->geom.custom_height = true;
    }
    
    RENDER(win) {
      graphics::drw_text(
        win->win_ref, props.font, props.color, props.text,
        state->geom.content_x().val(),
        (state->geom.content_y() + props.font->size).val()
      );
    }
  };
  
}// namespace primitives

#define rectangle(CLR, W, H, FILLED) \
new primitives::Rectangle(CLR,0,0,W,H,FILLED)

#endif//CYD_UI_PRIMITIVES_HPP
