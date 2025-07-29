/*! \file  keycode_map.cppm
 *! \brief
 *!
 */

module;
#include <SDL3/SDL_keycode.h>

#define MAP(A, B)                                                                                  \
  case A:                                                                                          \
    return B

#define MAP_SDL3(A)     MAP(SDLK_##A, Keycode::A)
#define MAP_SDL3_NUM(A) MAP(SDLK_##A, Keycode::NUM_##A)
#define MAP_SDL3_MOD(A) MAP(SDL_KMOD_##A, Keymod::A)

export module cydui.platform.window.sdl3.keycode_map;
export import cydui.platform.window.sdl3.scancode_map;

import std;
import reflect;

export import cydui.event_types;

namespace cydui::platform::window {
  export constexpr Keycode map_sdl3_keycode(SDL_Keycode keycode) {
    switch (keycode) {
      MAP_SDL3(UNKNOWN);
      MAP_SDL3(RETURN);
      MAP_SDL3(ESCAPE);
      MAP_SDL3(BACKSPACE);
      MAP_SDL3(TAB);
      MAP_SDL3(SPACE);
      MAP_SDL3(EXCLAIM);
      // MAP_SDL3(DBLAPOSTROPHE);
      MAP_SDL3(HASH);
      MAP_SDL3(DOLLAR);
      MAP_SDL3(PERCENT);
      MAP_SDL3(AMPERSAND);
      // MAP_SDL3(APOSTROPHE);
      MAP_SDL3(LEFTPAREN);
      MAP_SDL3(RIGHTPAREN);
      MAP_SDL3(ASTERISK);
      MAP_SDL3(PLUS);
      MAP_SDL3(COMMA);
      MAP_SDL3(MINUS);
      MAP_SDL3(PERIOD);
      MAP_SDL3(SLASH);
      MAP_SDL3_NUM(0);
      MAP_SDL3_NUM(1);
      MAP_SDL3_NUM(2);
      MAP_SDL3_NUM(3);
      MAP_SDL3_NUM(4);
      MAP_SDL3_NUM(5);
      MAP_SDL3_NUM(6);
      MAP_SDL3_NUM(7);
      MAP_SDL3_NUM(8);
      MAP_SDL3_NUM(9);
      MAP_SDL3(COLON);
      MAP_SDL3(SEMICOLON);
      MAP_SDL3(LESS);
      MAP_SDL3(EQUALS);
      MAP_SDL3(GREATER);
      MAP_SDL3(QUESTION);
      MAP_SDL3(AT);
      MAP_SDL3(LEFTBRACKET);
      MAP_SDL3(BACKSLASH);
      MAP_SDL3(RIGHTBRACKET);
      MAP_SDL3(CARET);
      MAP_SDL3(UNDERSCORE);
      // MAP_SDL3(GRAVE);
      MAP_SDL3(A);
      MAP_SDL3(B);
      MAP_SDL3(C);
      MAP_SDL3(D);
      MAP_SDL3(E);
      MAP_SDL3(F);
      MAP_SDL3(G);
      MAP_SDL3(H);
      MAP_SDL3(I);
      MAP_SDL3(J);
      MAP_SDL3(K);
      MAP_SDL3(L);
      MAP_SDL3(M);
      MAP_SDL3(N);
      MAP_SDL3(O);
      MAP_SDL3(P);
      MAP_SDL3(Q);
      MAP_SDL3(R);
      MAP_SDL3(S);
      MAP_SDL3(T);
      MAP_SDL3(U);
      MAP_SDL3(V);
      MAP_SDL3(W);
      MAP_SDL3(X);
      MAP_SDL3(Y);
      MAP_SDL3(Z);
      // MAP_SDL3(LEFTBRACE);
      // MAP_SDL3(PIPE);
      // MAP_SDL3(RIGHTBRACE);
      // MAP_SDL3(TILDE);
      MAP_SDL3(DELETE);
      // MAP_SDL3(PLUSMINUS);
      MAP_SDL3(CAPSLOCK);
      MAP_SDL3(F1);
      MAP_SDL3(F2);
      MAP_SDL3(F3);
      MAP_SDL3(F4);
      MAP_SDL3(F5);
      MAP_SDL3(F6);
      MAP_SDL3(F7);
      MAP_SDL3(F8);
      MAP_SDL3(F9);
      MAP_SDL3(F10);
      MAP_SDL3(F11);
      MAP_SDL3(F12);
      MAP_SDL3(PRINTSCREEN);
      MAP_SDL3(SCROLLLOCK);
      MAP_SDL3(PAUSE);
      MAP_SDL3(INSERT);
      MAP_SDL3(HOME);
      MAP_SDL3(PAGEUP);
      MAP_SDL3(END);
      MAP_SDL3(PAGEDOWN);
      MAP_SDL3(RIGHT);
      MAP_SDL3(LEFT);
      MAP_SDL3(DOWN);
      MAP_SDL3(UP);
      MAP_SDL3(NUMLOCKCLEAR);
      MAP_SDL3(KP_DIVIDE);
      MAP_SDL3(KP_MULTIPLY);
      MAP_SDL3(KP_MINUS);
      MAP_SDL3(KP_PLUS);
      MAP_SDL3(KP_ENTER);
      MAP_SDL3(KP_1);
      MAP_SDL3(KP_2);
      MAP_SDL3(KP_3);
      MAP_SDL3(KP_4);
      MAP_SDL3(KP_5);
      MAP_SDL3(KP_6);
      MAP_SDL3(KP_7);
      MAP_SDL3(KP_8);
      MAP_SDL3(KP_9);
      MAP_SDL3(KP_0);
      MAP_SDL3(KP_PERIOD);
      MAP_SDL3(APPLICATION);
      MAP_SDL3(POWER);
      MAP_SDL3(KP_EQUALS);
      MAP_SDL3(F13);
      MAP_SDL3(F14);
      MAP_SDL3(F15);
      MAP_SDL3(F16);
      MAP_SDL3(F17);
      MAP_SDL3(F18);
      MAP_SDL3(F19);
      MAP_SDL3(F20);
      MAP_SDL3(F21);
      MAP_SDL3(F22);
      MAP_SDL3(F23);
      MAP_SDL3(F24);
      MAP_SDL3(EXECUTE);
      MAP_SDL3(HELP);
      MAP_SDL3(MENU);
      MAP_SDL3(SELECT);
      MAP_SDL3(STOP);
      MAP_SDL3(AGAIN);
      MAP_SDL3(UNDO);
      MAP_SDL3(CUT);
      MAP_SDL3(COPY);
      MAP_SDL3(PASTE);
      MAP_SDL3(FIND);
      MAP_SDL3(MUTE);
      MAP_SDL3(VOLUMEUP);
      MAP_SDL3(VOLUMEDOWN);
      MAP_SDL3(KP_COMMA);
      MAP_SDL3(KP_EQUALSAS400);
      MAP_SDL3(ALTERASE);
      MAP_SDL3(SYSREQ);
      MAP_SDL3(CANCEL);
      MAP_SDL3(CLEAR);
      MAP_SDL3(PRIOR);
      MAP_SDL3(RETURN2);
      MAP_SDL3(SEPARATOR);
      MAP_SDL3(OUT);
      MAP_SDL3(OPER);
      MAP_SDL3(CLEARAGAIN);
      MAP_SDL3(CRSEL);
      MAP_SDL3(EXSEL);
      MAP_SDL3(KP_00);
      MAP_SDL3(KP_000);
      MAP_SDL3(THOUSANDSSEPARATOR);
      MAP_SDL3(DECIMALSEPARATOR);
      MAP_SDL3(CURRENCYUNIT);
      MAP_SDL3(CURRENCYSUBUNIT);
      MAP_SDL3(KP_LEFTPAREN);
      MAP_SDL3(KP_RIGHTPAREN);
      MAP_SDL3(KP_LEFTBRACE);
      MAP_SDL3(KP_RIGHTBRACE);
      MAP_SDL3(KP_TAB);
      MAP_SDL3(KP_BACKSPACE);
      MAP_SDL3(KP_A);
      MAP_SDL3(KP_B);
      MAP_SDL3(KP_C);
      MAP_SDL3(KP_D);
      MAP_SDL3(KP_E);
      MAP_SDL3(KP_F);
      MAP_SDL3(KP_XOR);
      MAP_SDL3(KP_POWER);
      MAP_SDL3(KP_PERCENT);
      MAP_SDL3(KP_LESS);
      MAP_SDL3(KP_GREATER);
      MAP_SDL3(KP_AMPERSAND);
      MAP_SDL3(KP_DBLAMPERSAND);
      MAP_SDL3(KP_VERTICALBAR);
      MAP_SDL3(KP_DBLVERTICALBAR);
      MAP_SDL3(KP_COLON);
      MAP_SDL3(KP_HASH);
      MAP_SDL3(KP_SPACE);
      MAP_SDL3(KP_AT);
      MAP_SDL3(KP_EXCLAM);
      MAP_SDL3(KP_MEMSTORE);
      MAP_SDL3(KP_MEMRECALL);
      MAP_SDL3(KP_MEMCLEAR);
      MAP_SDL3(KP_MEMADD);
      MAP_SDL3(KP_MEMSUBTRACT);
      MAP_SDL3(KP_MEMMULTIPLY);
      MAP_SDL3(KP_MEMDIVIDE);
      MAP_SDL3(KP_PLUSMINUS);
      MAP_SDL3(KP_CLEAR);
      MAP_SDL3(KP_CLEARENTRY);
      MAP_SDL3(KP_BINARY);
      MAP_SDL3(KP_OCTAL);
      MAP_SDL3(KP_DECIMAL);
      MAP_SDL3(KP_HEXADECIMAL);
      MAP_SDL3(LCTRL);
      MAP_SDL3(LSHIFT);
      MAP_SDL3(LALT);
      MAP_SDL3(LGUI);
      MAP_SDL3(RCTRL);
      MAP_SDL3(RSHIFT);
      MAP_SDL3(RALT);
      MAP_SDL3(RGUI);
      MAP_SDL3(MODE);
      MAP_SDL3(SLEEP);
      // MAP_SDL3(WAKE);
      // MAP_SDL3(CHANNEL_INCREMENT);
      // MAP_SDL3(CHANNEL_DECREMENT);
      // MAP_SDL3(MEDIA_PLAY);
      // MAP_SDL3(MEDIA_PAUSE);
      // MAP_SDL3(MEDIA_RECORD);
      // MAP_SDL3(MEDIA_FAST_FORWARD);
      // MAP_SDL3(MEDIA_REWIND);
      // MAP_SDL3(MEDIA_NEXT_TRACK);
      // MAP_SDL3(MEDIA_PREVIOUS_TRACK);
      // MAP_SDL3(MEDIA_STOP);
      // MAP_SDL3(MEDIA_EJECT);
      // MAP_SDL3(MEDIA_PLAY_PAUSE);
      // MAP_SDL3(MEDIA_SELECT);
      // MAP_SDL3(AC_NEW);
      // MAP_SDL3(AC_OPEN);
      // MAP_SDL3(AC_CLOSE);
      // MAP_SDL3(AC_EXIT);
      // MAP_SDL3(AC_SAVE);
      // MAP_SDL3(AC_PRINT);
      // MAP_SDL3(AC_PROPERTIES);
      MAP_SDL3(AC_SEARCH);
      MAP_SDL3(AC_HOME);
      MAP_SDL3(AC_BACK);
      MAP_SDL3(AC_FORWARD);
      MAP_SDL3(AC_STOP);
      MAP_SDL3(AC_REFRESH);
      MAP_SDL3(AC_BOOKMARKS);
      MAP_SDL3(SOFTLEFT);
      MAP_SDL3(SOFTRIGHT);
      MAP_SDL3(CALL);
      MAP_SDL3(ENDCALL);
      // MAP_SDL3(LEFT_TAB);
      // MAP_SDL3(LEVEL5_SHIFT);
      // MAP_SDL3(MULTI_KEY_COMPOSE);
      // MAP_SDL3(LMETA);
      // MAP_SDL3(RMETA);
      // MAP_SDL3(LHYPER);
      // MAP_SDL3(RHYPER);
      default:
        return Keycode::UNKNOWN;
    }
  }

  export constexpr Keymod map_sdl3_keymod(SDL_Keymod keymod) {
    switch (keymod) {
      MAP_SDL3_MOD(NONE);
      MAP_SDL3_MOD(LSHIFT);
      MAP_SDL3_MOD(RSHIFT);
      MAP_SDL3_MOD(LEVEL5);
      MAP_SDL3_MOD(LCTRL);
      MAP_SDL3_MOD(RCTRL);
      MAP_SDL3_MOD(LALT);
      MAP_SDL3_MOD(RALT);
      MAP_SDL3_MOD(LGUI);
      MAP_SDL3_MOD(RGUI);
      MAP_SDL3_MOD(NUM);
      MAP_SDL3_MOD(CAPS);
      MAP_SDL3_MOD(MODE);
      MAP_SDL3_MOD(SCROLL);
      MAP_SDL3_MOD(CTRL);
      MAP_SDL3_MOD(SHIFT);
      MAP_SDL3_MOD(ALT);
      MAP_SDL3_MOD(GUI);
      default:
        return Keymod::NONE;
    }
  }
} // namespace cydui::platform::window
