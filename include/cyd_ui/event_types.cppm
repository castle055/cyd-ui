// Copyright c) 2024, Víctor Castillo Agüero. // SPDX,License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>

#define EXTENDED_MASK          (1u << 29)
#define SCANCODE_MASK          (1u << 30)
#define SCANCODE_TO_KEYCODE(X) ((unsigned int)X | SCANCODE_MASK)

export module cydui.event_types;

export import std;
export import fabric.async;
export import cydui.dimensions;

using cydui::dimensions::screen_measure;

export namespace cydui {
  /**
   * These values are based on the USB usage page standard:
   * https://usb.org/sites/default/files/hut1_5.pdf
   */
  enum class Scancode : unsigned int {
    UNKNOWN = 0,

    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,

    NUM_1 = 30,
    NUM_2 = 31,
    NUM_3 = 32,
    NUM_4 = 33,
    NUM_5 = 34,
    NUM_6 = 35,
    NUM_7 = 36,
    NUM_8 = 37,
    NUM_9 = 38,
    NUM_0 = 39,

    RETURN    = 40,
    ESCAPE    = 41,
    BACKSPACE = 42,
    TAB       = 43,
    SPACE     = 44,

    MINUS        = 45,
    EQUALS       = 46,
    LEFTBRACKET  = 47,
    RIGHTBRACKET = 48,
    BACKSLASH    = 49,

    NONUSHASH = 50,

    SEMICOLON  = 51,
    APOSTROPHE = 52,
    GRAVE      = 53,

    COMMA  = 54,
    PERIOD = 55,
    SLASH  = 56,

    CAPSLOCK = 57,

    F1  = 58,
    F2  = 59,
    F3  = 60,
    F4  = 61,
    F5  = 62,
    F6  = 63,
    F7  = 64,
    F8  = 65,
    F9  = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,

    PRINTSCREEN = 70,
    SCROLLLOCK  = 71,
    PAUSE       = 72,
    INSERT      = 73,

    HOME     = 74,
    PAGEUP   = 75,
    DELETE   = 76,
    END      = 77,
    PAGEDOWN = 78,
    RIGHT    = 79,
    LEFT     = 80,
    DOWN     = 81,
    UP       = 82,

    NUMLOCKCLEAR = 83,

    KP_DIVIDE   = 84,
    KP_MULTIPLY = 85,
    KP_MINUS    = 86,
    KP_PLUS     = 87,
    KP_ENTER    = 88,
    KP_1        = 89,
    KP_2        = 90,
    KP_3        = 91,
    KP_4        = 92,
    KP_5        = 93,
    KP_6        = 94,
    KP_7        = 95,
    KP_8        = 96,
    KP_9        = 97,
    KP_0        = 98,
    KP_PERIOD   = 99,

    NONUSBACKSLASH = 100,
    APPLICATION    = 101,
    POWER          = 102,

    KP_EQUALS  = 103,
    F13        = 104,
    F14        = 105,
    F15        = 106,
    F16        = 107,
    F17        = 108,
    F18        = 109,
    F19        = 110,
    F20        = 111,
    F21        = 112,
    F22        = 113,
    F23        = 114,
    F24        = 115,
    EXECUTE    = 116,
    HELP       = 117,
    MENU       = 118,
    SELECT     = 119,
    STOP       = 120,
    AGAIN      = 121,
    UNDO       = 122,
    CUT        = 123,
    COPY       = 124,
    PASTE      = 125,
    FIND       = 126,
    MUTE       = 127,
    VOLUMEUP   = 128,
    VOLUMEDOWN = 129,

    KP_COMMA       = 133,
    KP_EQUALSAS400 = 134,

    INTERNATIONAL1 = 135,
    INTERNATIONAL2 = 136,
    INTERNATIONAL3 = 137,
    INTERNATIONAL4 = 138,
    INTERNATIONAL5 = 139,
    INTERNATIONAL6 = 140,
    INTERNATIONAL7 = 141,
    INTERNATIONAL8 = 142,
    INTERNATIONAL9 = 143,
    LANG1          = 144,
    LANG2          = 145,
    LANG3          = 146,
    LANG4          = 147,
    LANG5          = 148,
    LANG6          = 149,
    LANG7          = 150,
    LANG8          = 151,
    LANG9          = 152,

    ALTERASE   = 153,
    SYSREQ     = 154,
    CANCEL     = 155,
    CLEAR      = 156,
    PRIOR      = 157,
    RETURN2    = 158,
    SEPARATOR  = 159,
    OUT        = 160,
    OPER       = 161,
    CLEARAGAIN = 162,
    CRSEL      = 163,
    EXSEL      = 164,

    KP_00              = 176,
    KP_000             = 177,
    THOUSANDSSEPARATOR = 178,
    DECIMALSEPARATOR   = 179,
    CURRENCYUNIT       = 180,
    CURRENCYSUBUNIT    = 181,
    KP_LEFTPAREN       = 182,
    KP_RIGHTPAREN      = 183,
    KP_LEFTBRACE       = 184,
    KP_RIGHTBRACE      = 185,
    KP_TAB             = 186,
    KP_BACKSPACE       = 187,
    KP_A               = 188,
    KP_B               = 189,
    KP_C               = 190,
    KP_D               = 191,
    KP_E               = 192,
    KP_F               = 193,
    KP_XOR             = 194,
    KP_POWER           = 195,
    KP_PERCENT         = 196,
    KP_LESS            = 197,
    KP_GREATER         = 198,
    KP_AMPERSAND       = 199,
    KP_DBLAMPERSAND    = 200,
    KP_VERTICALBAR     = 201,
    KP_DBLVERTICALBAR  = 202,
    KP_COLON           = 203,
    KP_HASH            = 204,
    KP_SPACE           = 205,
    KP_AT              = 206,
    KP_EXCLAM          = 207,
    KP_MEMSTORE        = 208,
    KP_MEMRECALL       = 209,
    KP_MEMCLEAR        = 210,
    KP_MEMADD          = 211,
    KP_MEMSUBTRACT     = 212,
    KP_MEMMULTIPLY     = 213,
    KP_MEMDIVIDE       = 214,
    KP_PLUSMINUS       = 215,
    KP_CLEAR           = 216,
    KP_CLEARENTRY      = 217,
    KP_BINARY          = 218,
    KP_OCTAL           = 219,
    KP_DECIMAL         = 220,
    KP_HEXADECIMAL     = 221,

    LCTRL  = 224,
    LSHIFT = 225,
    LALT   = 226,
    LGUI   = 227,
    RCTRL  = 228,
    RSHIFT = 229,
    RALT   = 230,
    RGUI   = 231,

    MODE  = 257,
    SLEEP = 258,
    WAKE  = 259,

    CHANNEL_INCREMENT = 260,
    CHANNEL_DECREMENT = 261,

    MEDIA_PLAY           = 262,
    MEDIA_PAUSE          = 263,
    MEDIA_RECORD         = 264,
    MEDIA_FAST_FORWARD   = 265,
    MEDIA_REWIND         = 266,
    MEDIA_NEXT_TRACK     = 267,
    MEDIA_PREVIOUS_TRACK = 268,
    MEDIA_STOP           = 269,
    MEDIA_EJECT          = 270,
    MEDIA_PLAY_PAUSE     = 271,
    MEDIA_SELECT         = 272,

    AC_NEW        = 273,
    AC_OPEN       = 274,
    AC_CLOSE      = 275,
    AC_EXIT       = 276,
    AC_SAVE       = 277,
    AC_PRINT      = 278,
    AC_PROPERTIES = 279,
    AC_SEARCH     = 280,
    AC_HOME       = 281,
    AC_BACK       = 282,
    AC_FORWARD    = 283,
    AC_STOP       = 284,
    AC_REFRESH    = 285,
    AC_BOOKMARKS  = 286,


    SOFTLEFT  = 287,
    SOFTRIGHT = 288,
    CALL      = 289,
    ENDCALL   = 290,


    RESERVED = 400,
    COUNT    = 512

  };

  enum class Keycode {
    UNKNOWN = 0,

    RETURN     = '\r',
    ESCAPE     = '\x1B',
    BACKSPACE  = '\b',
    TAB        = '\t',
    SPACE      = ' ',
    EXCLAIM    = '!',
    QUOTEDBL   = '"',
    HASH       = '#',
    PERCENT    = '%',
    DOLLAR     = '$',
    AMPERSAND  = '&',
    QUOTE      = '\'',
    LEFTPAREN  = '(',
    RIGHTPAREN = ')',
    ASTERISK   = '*',
    PLUS       = '+',
    COMMA      = ',',
    MINUS      = '-',
    PERIOD     = '.',
    SLASH      = '/',
    NUM_0      = '0',
    NUM_1      = '1',
    NUM_2      = '2',
    NUM_3      = '3',
    NUM_4      = '4',
    NUM_5      = '5',
    NUM_6      = '6',
    NUM_7      = '7',
    NUM_8      = '8',
    NUM_9      = '9',
    COLON      = ':',
    SEMICOLON  = ';',
    LESS       = '<',
    EQUALS     = '=',
    GREATER    = '>',
    QUESTION   = '?',
    AT         = '@',

    /*
       Skip uppercase letters
     */

    LEFTBRACKET  = '[',
    BACKSLASH    = '\\',
    RIGHTBRACKET = ']',
    CARET        = '^',
    UNDERSCORE   = '_',
    BACKQUOTE    = '`',
    A            = 'a',
    B            = 'b',
    C            = 'c',
    D            = 'd',
    E            = 'e',
    F            = 'f',
    G            = 'g',
    H            = 'h',
    I            = 'i',
    J            = 'j',
    K            = 'k',
    L            = 'l',
    M            = 'm',
    N            = 'n',
    O            = 'o',
    P            = 'p',
    Q            = 'q',
    R            = 'r',
    S            = 's',
    T            = 't',
    U            = 'u',
    V            = 'v',
    W            = 'w',
    X            = 'x',
    Y            = 'y',
    Z            = 'z',

    CAPSLOCK = SCANCODE_TO_KEYCODE(Scancode::CAPSLOCK),

    F1  = SCANCODE_TO_KEYCODE(Scancode::F1),
    F2  = SCANCODE_TO_KEYCODE(Scancode::F2),
    F3  = SCANCODE_TO_KEYCODE(Scancode::F3),
    F4  = SCANCODE_TO_KEYCODE(Scancode::F4),
    F5  = SCANCODE_TO_KEYCODE(Scancode::F5),
    F6  = SCANCODE_TO_KEYCODE(Scancode::F6),
    F7  = SCANCODE_TO_KEYCODE(Scancode::F7),
    F8  = SCANCODE_TO_KEYCODE(Scancode::F8),
    F9  = SCANCODE_TO_KEYCODE(Scancode::F9),
    F10 = SCANCODE_TO_KEYCODE(Scancode::F10),
    F11 = SCANCODE_TO_KEYCODE(Scancode::F11),
    F12 = SCANCODE_TO_KEYCODE(Scancode::F12),

    PRINTSCREEN = SCANCODE_TO_KEYCODE(Scancode::PRINTSCREEN),
    SCROLLLOCK  = SCANCODE_TO_KEYCODE(Scancode::SCROLLLOCK),
    PAUSE       = SCANCODE_TO_KEYCODE(Scancode::PAUSE),
    INSERT      = SCANCODE_TO_KEYCODE(Scancode::INSERT),
    HOME        = SCANCODE_TO_KEYCODE(Scancode::HOME),
    PAGEUP      = SCANCODE_TO_KEYCODE(Scancode::PAGEUP),
    DELETE      = '\x7F',
    END         = SCANCODE_TO_KEYCODE(Scancode::END),
    PAGEDOWN    = SCANCODE_TO_KEYCODE(Scancode::PAGEDOWN),
    RIGHT       = SCANCODE_TO_KEYCODE(Scancode::RIGHT),
    LEFT        = SCANCODE_TO_KEYCODE(Scancode::LEFT),
    DOWN        = SCANCODE_TO_KEYCODE(Scancode::DOWN),
    UP          = SCANCODE_TO_KEYCODE(Scancode::UP),

    NUMLOCKCLEAR = SCANCODE_TO_KEYCODE(Scancode::NUMLOCKCLEAR),
    KP_DIVIDE    = SCANCODE_TO_KEYCODE(Scancode::KP_DIVIDE),
    KP_MULTIPLY  = SCANCODE_TO_KEYCODE(Scancode::KP_MULTIPLY),
    KP_MINUS     = SCANCODE_TO_KEYCODE(Scancode::KP_MINUS),
    KP_PLUS      = SCANCODE_TO_KEYCODE(Scancode::KP_PLUS),
    KP_ENTER     = SCANCODE_TO_KEYCODE(Scancode::KP_ENTER),
    KP_1         = SCANCODE_TO_KEYCODE(Scancode::KP_1),
    KP_2         = SCANCODE_TO_KEYCODE(Scancode::KP_2),
    KP_3         = SCANCODE_TO_KEYCODE(Scancode::KP_3),
    KP_4         = SCANCODE_TO_KEYCODE(Scancode::KP_4),
    KP_5         = SCANCODE_TO_KEYCODE(Scancode::KP_5),
    KP_6         = SCANCODE_TO_KEYCODE(Scancode::KP_6),
    KP_7         = SCANCODE_TO_KEYCODE(Scancode::KP_7),
    KP_8         = SCANCODE_TO_KEYCODE(Scancode::KP_8),
    KP_9         = SCANCODE_TO_KEYCODE(Scancode::KP_9),
    KP_0         = SCANCODE_TO_KEYCODE(Scancode::KP_0),
    KP_PERIOD    = SCANCODE_TO_KEYCODE(Scancode::KP_PERIOD),

    APPLICATION    = SCANCODE_TO_KEYCODE(Scancode::APPLICATION),
    POWER          = SCANCODE_TO_KEYCODE(Scancode::POWER),
    KP_EQUALS      = SCANCODE_TO_KEYCODE(Scancode::KP_EQUALS),
    F13            = SCANCODE_TO_KEYCODE(Scancode::F13),
    F14            = SCANCODE_TO_KEYCODE(Scancode::F14),
    F15            = SCANCODE_TO_KEYCODE(Scancode::F15),
    F16            = SCANCODE_TO_KEYCODE(Scancode::F16),
    F17            = SCANCODE_TO_KEYCODE(Scancode::F17),
    F18            = SCANCODE_TO_KEYCODE(Scancode::F18),
    F19            = SCANCODE_TO_KEYCODE(Scancode::F19),
    F20            = SCANCODE_TO_KEYCODE(Scancode::F20),
    F21            = SCANCODE_TO_KEYCODE(Scancode::F21),
    F22            = SCANCODE_TO_KEYCODE(Scancode::F22),
    F23            = SCANCODE_TO_KEYCODE(Scancode::F23),
    F24            = SCANCODE_TO_KEYCODE(Scancode::F24),
    EXECUTE        = SCANCODE_TO_KEYCODE(Scancode::EXECUTE),
    HELP           = SCANCODE_TO_KEYCODE(Scancode::HELP),
    MENU           = SCANCODE_TO_KEYCODE(Scancode::MENU),
    SELECT         = SCANCODE_TO_KEYCODE(Scancode::SELECT),
    STOP           = SCANCODE_TO_KEYCODE(Scancode::STOP),
    AGAIN          = SCANCODE_TO_KEYCODE(Scancode::AGAIN),
    UNDO           = SCANCODE_TO_KEYCODE(Scancode::UNDO),
    CUT            = SCANCODE_TO_KEYCODE(Scancode::CUT),
    COPY           = SCANCODE_TO_KEYCODE(Scancode::COPY),
    PASTE          = SCANCODE_TO_KEYCODE(Scancode::PASTE),
    FIND           = SCANCODE_TO_KEYCODE(Scancode::FIND),
    MUTE           = SCANCODE_TO_KEYCODE(Scancode::MUTE),
    VOLUMEUP       = SCANCODE_TO_KEYCODE(Scancode::VOLUMEUP),
    VOLUMEDOWN     = SCANCODE_TO_KEYCODE(Scancode::VOLUMEDOWN),
    KP_COMMA       = SCANCODE_TO_KEYCODE(Scancode::KP_COMMA),
    KP_EQUALSAS400 = SCANCODE_TO_KEYCODE(Scancode::KP_EQUALSAS400),

    ALTERASE   = SCANCODE_TO_KEYCODE(Scancode::ALTERASE),
    SYSREQ     = SCANCODE_TO_KEYCODE(Scancode::SYSREQ),
    CANCEL     = SCANCODE_TO_KEYCODE(Scancode::CANCEL),
    CLEAR      = SCANCODE_TO_KEYCODE(Scancode::CLEAR),
    PRIOR      = SCANCODE_TO_KEYCODE(Scancode::PRIOR),
    RETURN2    = SCANCODE_TO_KEYCODE(Scancode::RETURN2),
    SEPARATOR  = SCANCODE_TO_KEYCODE(Scancode::SEPARATOR),
    OUT        = SCANCODE_TO_KEYCODE(Scancode::OUT),
    OPER       = SCANCODE_TO_KEYCODE(Scancode::OPER),
    CLEARAGAIN = SCANCODE_TO_KEYCODE(Scancode::CLEARAGAIN),
    CRSEL      = SCANCODE_TO_KEYCODE(Scancode::CRSEL),
    EXSEL      = SCANCODE_TO_KEYCODE(Scancode::EXSEL),

    KP_00              = SCANCODE_TO_KEYCODE(Scancode::KP_00),
    KP_000             = SCANCODE_TO_KEYCODE(Scancode::KP_000),
    THOUSANDSSEPARATOR = SCANCODE_TO_KEYCODE(Scancode::THOUSANDSSEPARATOR),
    DECIMALSEPARATOR   = SCANCODE_TO_KEYCODE(Scancode::DECIMALSEPARATOR),
    CURRENCYUNIT       = SCANCODE_TO_KEYCODE(Scancode::CURRENCYUNIT),
    CURRENCYSUBUNIT    = SCANCODE_TO_KEYCODE(Scancode::CURRENCYSUBUNIT),
    KP_LEFTPAREN       = SCANCODE_TO_KEYCODE(Scancode::KP_LEFTPAREN),
    KP_RIGHTPAREN      = SCANCODE_TO_KEYCODE(Scancode::KP_RIGHTPAREN),
    KP_LEFTBRACE       = SCANCODE_TO_KEYCODE(Scancode::KP_LEFTBRACE),
    KP_RIGHTBRACE      = SCANCODE_TO_KEYCODE(Scancode::KP_RIGHTBRACE),
    KP_TAB             = SCANCODE_TO_KEYCODE(Scancode::KP_TAB),
    KP_BACKSPACE       = SCANCODE_TO_KEYCODE(Scancode::KP_BACKSPACE),
    KP_A               = SCANCODE_TO_KEYCODE(Scancode::KP_A),
    KP_B               = SCANCODE_TO_KEYCODE(Scancode::KP_B),
    KP_C               = SCANCODE_TO_KEYCODE(Scancode::KP_C),
    KP_D               = SCANCODE_TO_KEYCODE(Scancode::KP_D),
    KP_E               = SCANCODE_TO_KEYCODE(Scancode::KP_E),
    KP_F               = SCANCODE_TO_KEYCODE(Scancode::KP_F),
    KP_XOR             = SCANCODE_TO_KEYCODE(Scancode::KP_XOR),
    KP_POWER           = SCANCODE_TO_KEYCODE(Scancode::KP_POWER),
    KP_PERCENT         = SCANCODE_TO_KEYCODE(Scancode::KP_PERCENT),
    KP_LESS            = SCANCODE_TO_KEYCODE(Scancode::KP_LESS),
    KP_GREATER         = SCANCODE_TO_KEYCODE(Scancode::KP_GREATER),
    KP_AMPERSAND       = SCANCODE_TO_KEYCODE(Scancode::KP_AMPERSAND),
    KP_DBLAMPERSAND    = SCANCODE_TO_KEYCODE(Scancode::KP_DBLAMPERSAND),
    KP_VERTICALBAR     = SCANCODE_TO_KEYCODE(Scancode::KP_VERTICALBAR),
    KP_DBLVERTICALBAR  = SCANCODE_TO_KEYCODE(Scancode::KP_DBLVERTICALBAR),
    KP_COLON           = SCANCODE_TO_KEYCODE(Scancode::KP_COLON),
    KP_HASH            = SCANCODE_TO_KEYCODE(Scancode::KP_HASH),
    KP_SPACE           = SCANCODE_TO_KEYCODE(Scancode::KP_SPACE),
    KP_AT              = SCANCODE_TO_KEYCODE(Scancode::KP_AT),
    KP_EXCLAM          = SCANCODE_TO_KEYCODE(Scancode::KP_EXCLAM),
    KP_MEMSTORE        = SCANCODE_TO_KEYCODE(Scancode::KP_MEMSTORE),
    KP_MEMRECALL       = SCANCODE_TO_KEYCODE(Scancode::KP_MEMRECALL),
    KP_MEMCLEAR        = SCANCODE_TO_KEYCODE(Scancode::KP_MEMCLEAR),
    KP_MEMADD          = SCANCODE_TO_KEYCODE(Scancode::KP_MEMADD),
    KP_MEMSUBTRACT     = SCANCODE_TO_KEYCODE(Scancode::KP_MEMSUBTRACT),
    KP_MEMMULTIPLY     = SCANCODE_TO_KEYCODE(Scancode::KP_MEMMULTIPLY),
    KP_MEMDIVIDE       = SCANCODE_TO_KEYCODE(Scancode::KP_MEMDIVIDE),
    KP_PLUSMINUS       = SCANCODE_TO_KEYCODE(Scancode::KP_PLUSMINUS),
    KP_CLEAR           = SCANCODE_TO_KEYCODE(Scancode::KP_CLEAR),
    KP_CLEARENTRY      = SCANCODE_TO_KEYCODE(Scancode::KP_CLEARENTRY),
    KP_BINARY          = SCANCODE_TO_KEYCODE(Scancode::KP_BINARY),
    KP_OCTAL           = SCANCODE_TO_KEYCODE(Scancode::KP_OCTAL),
    KP_DECIMAL         = SCANCODE_TO_KEYCODE(Scancode::KP_DECIMAL),
    KP_HEXADECIMAL     = SCANCODE_TO_KEYCODE(Scancode::KP_HEXADECIMAL),

    LCTRL  = SCANCODE_TO_KEYCODE(Scancode::LCTRL),
    LSHIFT = SCANCODE_TO_KEYCODE(Scancode::LSHIFT),
    LALT   = SCANCODE_TO_KEYCODE(Scancode::LALT),
    LGUI   = SCANCODE_TO_KEYCODE(Scancode::LGUI),
    RCTRL  = SCANCODE_TO_KEYCODE(Scancode::RCTRL),
    RSHIFT = SCANCODE_TO_KEYCODE(Scancode::RSHIFT),
    RALT   = SCANCODE_TO_KEYCODE(Scancode::RALT),
    RGUI   = SCANCODE_TO_KEYCODE(Scancode::RGUI),

    MODE = SCANCODE_TO_KEYCODE(Scancode::MODE),

    AC_SEARCH    = SCANCODE_TO_KEYCODE(Scancode::AC_SEARCH),
    AC_HOME      = SCANCODE_TO_KEYCODE(Scancode::AC_HOME),
    AC_BACK      = SCANCODE_TO_KEYCODE(Scancode::AC_BACK),
    AC_FORWARD   = SCANCODE_TO_KEYCODE(Scancode::AC_FORWARD),
    AC_STOP      = SCANCODE_TO_KEYCODE(Scancode::AC_STOP),
    AC_REFRESH   = SCANCODE_TO_KEYCODE(Scancode::AC_REFRESH),
    AC_BOOKMARKS = SCANCODE_TO_KEYCODE(Scancode::AC_BOOKMARKS),

    SLEEP = SCANCODE_TO_KEYCODE(Scancode::SLEEP),

    SOFTLEFT  = SCANCODE_TO_KEYCODE(Scancode::SOFTLEFT),
    SOFTRIGHT = SCANCODE_TO_KEYCODE(Scancode::SOFTRIGHT),
    CALL      = SCANCODE_TO_KEYCODE(Scancode::CALL),
    ENDCALL   = SCANCODE_TO_KEYCODE(Scancode::ENDCALL)
  };

  enum class Keymod : unsigned short {
    NONE   = 0x0000u, /**< no modifier is applicable. */
    LSHIFT = 0x0001u, /**< the left Shift key is down. */
    RSHIFT = 0x0002u, /**< the right Shift key is down. */
    LEVEL5 = 0x0004u, /**< the Level 5 Shift key is down. */
    LCTRL  = 0x0040u, /**< the left Ctrl (Control) key is down. */
    RCTRL  = 0x0080u, /**< the right Ctrl (Control) key is down. */
    LALT   = 0x0100u, /**< the left Alt key is down. */
    RALT   = 0x0200u, /**< the right Alt key is down. */
    LGUI   = 0x0400u, /**< the left GUI key (often the Windows key) is down. */
    RGUI   = 0x0800u, /**< the right GUI key (often the Windows key) is down. */
    NUM    = 0x1000u, /**< the Num Lock key (may be located on an extended keypad) is down. */
    CAPS   = 0x2000u, /**< the Caps Lock key is down. */
    MODE   = 0x4000u, /**< the !AltGr key is down. */
    SCROLL = 0x8000u, /**< the Scroll Lock key is down. */
    CTRL   = (LCTRL | RCTRL),   /**< Any Ctrl key is down. */
    SHIFT  = (LSHIFT | RSHIFT), /**< Any Shift key is down. */
    ALT    = (LALT | RALT),     /**< Any Alt key is down. */
    GUI    = (LGUI | RGUI),     /**< Any GUI key is down. */
  };

  EVENT(RedrawEvent) {
    unsigned long win       = 0;
    int           x         = 0;
    int           y         = 0;
    void*         component = nullptr; // Points to the target `mounted_component_t`
  };

  EVENT(ButtonEvent) {
    const unsigned long  win      = 0;
    const unsigned int   button   = 0;
    const screen_measure x        = 0;
    const screen_measure y        = 0;
    const bool           pressed  = false;
    const bool           released = false;
    const bool           holding  = false;
  };

  EVENT(ScrollEvent) {
    unsigned long        win = 0;
    const screen_measure dy  = 0;
    const screen_measure dx  = 0;
    const screen_measure x   = 0;
    const screen_measure y   = 0;
  };

  EVENT(MotionEvent) {
    unsigned long        win   = 0;
    const screen_measure x     = 0;
    const screen_measure y     = 0;
    bool                 enter = false;
    bool                 exit  = false;

    bool dragging = false;
  };

  EVENT(ResizeEvent) {
    unsigned long        win = 0;
    const screen_measure w   = 0;
    const screen_measure h   = 0;
  };

  enum class Button {
    PRIMARY   = 1,
    SECONDARY = 3,
    WHEEL     = 2,
    BUTTON1   = PRIMARY,
    BUTTON2   = WHEEL,
    BUTTON3   = SECONDARY,
  };

  EVENT(KeyEvent) {
    Scancode   scancode;
    Keycode    code;
    Keymod     modifier;
    const bool pressed  = false;
    const bool released = false;
    const bool holding  = false;
  };

  EVENT(TextInputEvent) {
    std::string text              = "";
    bool        compositing_event = false;
    struct {
      int cursor{0};
      int selection{0};
    } compositing_state{};
  };

  EVENT(WindowCloseRequested){};
  EVENT(WindowClosed){};

  EVENT(DebugKeyPressed){};
} // namespace cydui
