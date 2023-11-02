//
// Created by castle on 10/26/23.
//

#ifndef CYD_UI_PRAGMA_H
#define CYD_UI_PRAGMA_H

#define DO_PRAGMA(x) _Pragma(#x)


#define TODO(...) DO_PRAGMA(message ("TODO - " #__VA_ARGS__))

#endif //CYD_UI_PRAGMA_H
