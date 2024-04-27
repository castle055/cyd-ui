// Copyright (c) 2024, Victor Castillo, All rights reserved.

//
// Created by castle on 8/23/22.
//

#ifndef CYD_UI_RENDER_HPP
#define CYD_UI_RENDER_HPP

namespace x11::render {
    void resize(pixelmap_t* target, int w, int h);
    
    void flush(cyd::ui::graphics::window_t* win);
}// namespace render


#endif//CYD_UI_RENDER_HPP
