//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_RENDER_HPP
#define CYD_UI_RENDER_HPP

namespace render {
    struct RenderThreadData {
      cydui::graphics::window_t* win;
      XImage* image = (XImage*) malloc(sizeof(XImage));
    };
    
    void start(cydui::graphics::window_t* win);
    
    void resize(pixelmap_t* target, int w, int h);
    
    void flush(cydui::graphics::window_t* win);
}// namespace render


#endif//CYD_UI_RENDER_HPP
