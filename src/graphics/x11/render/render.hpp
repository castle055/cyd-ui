// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_RENDER_HPP
#define CYD_UI_RENDER_HPP

namespace x11::render {
    void resize(pixelmap_t* target, int w, int h);
    
    void flush(cyd::ui::graphics::window_t* win);
}// namespace render


#endif//CYD_UI_RENDER_HPP
