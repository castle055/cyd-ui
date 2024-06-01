// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_STATE_HPP
#define CYD_UI_STATE_HPP


#include "../../driver.h"

#include <X11/Xlib.h>
#include "../x11_impl.hpp"

namespace x11::state {
    Display* get_dpy();
    
    int get_screen();
    
    Window get_root();
}// namespace state


#endif//CYD_UI_STATE_HPP
