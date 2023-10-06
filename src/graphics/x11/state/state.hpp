//
// Created by castle on 8/21/22.
//

#ifndef CYD_UI_STATE_HPP
#define CYD_UI_STATE_HPP


#include "../../driver.h"

#include <X11/Xlib.h>

namespace state {
    Display* get_dpy();
    
    int get_screen();
    
    Window get_root();
}// namespace state


#endif//CYD_UI_STATE_HPP
