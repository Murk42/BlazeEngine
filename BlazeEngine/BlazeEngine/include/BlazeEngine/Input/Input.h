#pragma once
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/String/String.h"

namespace Blaze
{
    class Window;
}

namespace Blaze::Input
{
    struct InputPreUpdateEvent {};
    struct InputPostUpdateEvent {};

    BLAZE_API EventDispatcher<InputPreUpdateEvent>& GetInputPreUpdateEventDispatcher();
    BLAZE_API EventDispatcher<InputPostUpdateEvent>& GetInputPostUpdateEventDispatcher();

    BLAZE_API void Update();

    BLAZE_API Vec2f GetFocusedWindowLastUpdateMousePos();
    BLAZE_API Vec2f GetFocusedWindowLastUpdateMouseMovementSum();

    BLAZE_API bool HasClipboardText();
    BLAZE_API u8String GetClipboardText();
    BLAZE_API void SetClipboardText(u8StringView text);

    //Key represents a physical button on the keyboard. It does not depent on the language
//or keyboard type or the active/pressed key modifiers.
    enum class Key : uint16
    {
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

        NUMBER1 = 30,
        NUMBER2 = 31,
        NUMBER3 = 32,
        NUMBER4 = 33,
        NUMBER5 = 34,
        NUMBER6 = 35,
        NUMBER7 = 36,
        NUMBER8 = 37,
        NUMBER9 = 38,
        NUMBER0 = 39,

        RETURN = 40,
        ENTER = RETURN,
        ESCAPE = 41,
        BACKSPACE = 42,
        TAB = 43,
        SPACE = 44,

        MINUS = 45,
        EQUALS = 46,
        LEFTBRACKET = 47,
        RIGHTBRACKET = 48,
        BACKSLASH = 49, /**< Located at the lower left of the return
                                      *   key on ISO keyboards and at the right end
                                      *   of the QWERTY row on ANSI keyboards.
                                      *   Produces REVERSE SOLIDUS (backslash) and
                                      *   VERTICAL LINE in a US layout, REVERSE
                                      *   SOLIDUS and VERTICAL LINE in a UK Mac
                                      *   layout, NUMBER SIGN and TILDE in a UK
                                      *   Windows layout, DOLLAR SIGN and POUND SIGN
                                      *   in a Swiss German layout, NUMBER SIGN and
                                      *   APOSTROPHE in a German layout, GRAVE
                                      *   ACCENT and POUND SIGN in a French Mac
                                      *   layout, and ASTERISK and MICRO SIGN in a
                                      *   French Windows layout.
                                      */
        NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                      *   instead of 49 for the same key, but all
                                      *   OSes I've seen treat the two codes
                                      *   identically. So, as an implementor, unless
                                      *   your keyboard generates both of those
                                      *   codes and your OS treats them differently,
                                      *   you should generate BACKSLASH
                                      *   instead of this code. As a user, you
                                      *   should not rely on this code because SDL
                                      *   will never generate it with most (all?)
                                      *   keyboards.
                                      */
        SEMICOLON = 51,
        APOSTROPHE = 52,
        GRAVE = 53, /**< Located in the top left corner (on both ANSI
                                  *   and ISO keyboards). Produces GRAVE ACCENT and
                                  *   TILDE in a US Windows layout and in US and UK
                                  *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                  *   and NOT SIGN in a UK Windows layout, SECTION
                                  *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                  *   layouts on ISO keyboards, SECTION SIGN and
                                  *   DEGREE SIGN in a Swiss German layout (Mac:
                                  *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                  *   DEGREE SIGN in a German layout (Mac: only on
                                  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                  *   French Windows layout, COMMERCIAL AT and
                                  *   NUMBER SIGN in a French Mac layout on ISO
                                  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                  *   SIGN in a Swiss German, German, or French Mac
                                  *   layout on ANSI keyboards.
                                  */
        COMMA = 54,
        PERIOD = 55,
        SLASH = 56,

        CAPSLOCK = 57,

        F1 = 58,
        F2 = 59,
        F3 = 60,
        F4 = 61,
        F5 = 62,
        F6 = 63,
        F7 = 64,
        F8 = 65,
        F9 = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        PRINTSCREEN = 70,
        SCROLLLOCK = 71,
        PAUSE = 72,
        INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                       does send code 73, not 117) */
        HOME = 74,
        PAGEUP = 75,
        DELETE = 76,
        END = 77,
        PAGEDOWN = 78,
        RIGHT = 79,
        LEFT = 80,
        DOWN = 81,
        UP = 82,

        NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                         */
        KP_DIVIDE = 84,
        KP_MULTIPLY = 85,
        KP_MINUS = 86,
        KP_PLUS = 87,
        KP_ENTER = 88,
        KP_1 = 89,
        KP_2 = 90,
        KP_3 = 91,
        KP_4 = 92,
        KP_5 = 93,
        KP_6 = 94,
        KP_7 = 95,
        KP_8 = 96,
        KP_9 = 97,
        KP_0 = 98,
        KP_PERIOD = 99,

        NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                            *   keyboards have over ANSI ones,
                                            *   located between left shift and Y.
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a
                                            *   US or UK Windows layout, and
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        APPLICATION = 101, /**< windows contextual menu, compose */
        POWER = 102, /**< The USB document says this is a status flag,
                                   *   not a physical key - but some Mac keyboards
                                   *   do have a power key. */
        KP_EQUALS = 103,
        F13 = 104,
        F14 = 105,
        F15 = 106,
        F16 = 107,
        F17 = 108,
        F18 = 109,
        F19 = 110,
        F20 = 111,
        F21 = 112,
        F22 = 113,
        F23 = 114,
        F24 = 115,
        EXECUTE = 116,
        HELP = 117,    /**< AL Integrated Help Center */
        MENU = 118,    /**< Menu (show menu) */
        SELECT = 119,
        STOP = 120,    /**< AC Stop */
        AGAIN = 121,   /**< AC Redo/Repeat */
        UNDO = 122,    /**< AC Undo */
        CUT = 123,     /**< AC Cut */
        COPY = 124,    /**< AC Copy */
        PASTE = 125,   /**< AC Paste */
        FIND = 126,    /**< AC Find */
        MUTE = 127,
        VOLUMEUP = 128,
        VOLUMEDOWN = 129,
        /* not sure whether there's a reason to enable these */
        /*     LOCKINGCAPSLOCK = 130,  */
        /*     LOCKINGNUMLOCK = 131, */
        /*     LOCKINGSCROLLLOCK = 132, */
        KP_COMMA = 133,
        KP_EQUALSAS400 = 134,

        INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                                footnotes in USB doc */
        INTERNATIONAL2 = 136,
        INTERNATIONAL3 = 137, /**< Yen */
        INTERNATIONAL4 = 138,
        INTERNATIONAL5 = 139,
        INTERNATIONAL6 = 140,
        INTERNATIONAL7 = 141,
        INTERNATIONAL8 = 142,
        INTERNATIONAL9 = 143,
        LANG1 = 144, /**< Hangul/English toggle */
        LANG2 = 145, /**< Hanja conversion */
        LANG3 = 146, /**< Katakana */
        LANG4 = 147, /**< Hiragana */
        LANG5 = 148, /**< Zenkaku/Hankaku */
        LANG6 = 149, /**< reserved */
        LANG7 = 150, /**< reserved */
        LANG8 = 151, /**< reserved */
        LANG9 = 152, /**< reserved */

        ALTERASE = 153,    /**< Erase-Eaze */
        SYSREQ = 154,
        CANCEL = 155,      /**< AC Cancel */
        CLEAR = 156,
        PRIOR = 157,
        RETURN2 = 158,
        SEPARATOR = 159,
        OUT = 160,
        OPER = 161,
        CLEARAGAIN = 162,
        CRSEL = 163,
        EXSEL = 164,

        KP_00 = 176,
        KP_000 = 177,
        THOUSANDSSEPARATOR = 178,
        DECIMALSEPARATOR = 179,
        CURRENCYUNIT = 180,
        CURRENCYSUBUNIT = 181,
        KP_LEFTPAREN = 182,
        KP_RIGHTPAREN = 183,
        KP_LEFTBRACE = 184,
        KP_RIGHTBRACE = 185,
        KP_TAB = 186,
        KP_BACKSPACE = 187,
        KP_A = 188,
        KP_B = 189,
        KP_C = 190,
        KP_D = 191,
        KP_E = 192,
        KP_F = 193,
        KP_XOR = 194,
        KP_POWER = 195,
        KP_PERCENT = 196,
        KP_LESS = 197,
        KP_GREATER = 198,
        KP_AMPERSAND = 199,
        KP_DBLAMPERSAND = 200,
        KP_VERTICALBAR = 201,
        KP_DBLVERTICALBAR = 202,
        KP_COLON = 203,
        KP_HASH = 204,
        KP_SPACE = 205,
        KP_AT = 206,
        KP_EXCLAM = 207,
        KP_MEMSTORE = 208,
        KP_MEMRECALL = 209,
        KP_MEMCLEAR = 210,
        KP_MEMADD = 211,
        KP_MEMSUBTRACT = 212,
        KP_MEMMULTIPLY = 213,
        KP_MEMDIVIDE = 214,
        KP_PLUSMINUS = 215,
        KP_CLEAR = 216,
        KP_CLEARENTRY = 217,
        KP_BINARY = 218,
        KP_OCTAL = 219,
        KP_DECIMAL = 220,
        KP_HEXADECIMAL = 221,

        LCTRL = 224,
        LSHIFT = 225,
        LALT = 226, /**< alt, option */
        LGUI = 227, /**< windows, command (apple), meta */
        RCTRL = 228,
        RSHIFT = 229,
        RALT = 230, /**< alt gr, option */
        RGUI = 231, /**< windows, command (apple), meta */

        MODE = 257,    /**< I'm not sure if this is really not covered
                                     *   by any of the above, but since there's a
                                     *   special SDL_KMOD_MODE for it I'm adding it here
                                     */

                                     /* @} *//* Usage page 0x07 */

                                     /**
                                      *  \name Usage page 0x0C
                                      *
                                      *  These values are mapped from usage page 0x0C (USB consumer page).
                                      *
                                      *  There are way more keys in the spec than we can represent in the
                                      *  current scancode range, so pick the ones that commonly come up in
                                      *  real world usage.
                                      */
                                      /* @{ */

        SLEEP = 258,                   /**< Sleep */
        WAKE = 259,                    /**< Wake */

        CHANNEL_INCREMENT = 260,       /**< Channel Increment */
        CHANNEL_DECREMENT = 261,       /**< Channel Decrement */

        MEDIA_PLAY = 262,          /**< Play */
        MEDIA_PAUSE = 263,         /**< Pause */
        MEDIA_RECORD = 264,        /**< Record */
        MEDIA_FAST_FORWARD = 265,  /**< Fast Forward */
        MEDIA_REWIND = 266,        /**< Rewind */
        MEDIA_NEXT_TRACK = 267,    /**< Next Track */
        MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
        MEDIA_STOP = 269,          /**< Stop */
        MEDIA_EJECT = 270,         /**< Eject */
        MEDIA_PLAY_PAUSE = 271,    /**< Play / Pause */
        MEDIA_SELECT = 272,        /* Media Select */

        AC_NEW = 273,              /**< AC New */
        AC_OPEN = 274,             /**< AC Open */
        AC_CLOSE = 275,            /**< AC Close */
        AC_EXIT = 276,             /**< AC Exit */
        AC_SAVE = 277,             /**< AC Save */
        AC_PRINT = 278,            /**< AC Print */
        AC_PROPERTIES = 279,       /**< AC Properties */

        AC_SEARCH = 280,           /**< AC Search */
        AC_HOME = 281,             /**< AC Home */
        AC_BACK = 282,             /**< AC Back */
        AC_FORWARD = 283,          /**< AC Forward */
        AC_STOP = 284,             /**< AC Stop */
        AC_REFRESH = 285,          /**< AC Refresh */
        AC_BOOKMARKS = 286,        /**< AC Bookmarks */

        /* @} *//* Usage page 0x0C */


        /**
         *  \name Mobile keys
         *
         *  These are values that are often used on mobile phones.
         */
         /* @{ */

        SOFTLEFT = 287, /**< Usually situated below the display on phones and
                                          used as a multi-function feature key for selecting
                                          a software defined function shown on the bottom left
                                          of the display. */
        SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                           used as a multi-function feature key for selecting
                                           a software defined function shown on the bottom right
                                           of the display. */
        CALL = 289, /**< Used for accepting phone calls. */
        ENDCALL = 290, /**< Used for rejecting phone calls. */

        /* @} *//* Mobile keys */

        /* Add any other keys here. */

        RESERVED = 400,    /**< 400-500 reserved for dynamic keycodes */

        COUNT = 512 /**< not a key, just marks the number of scancodes for array bounds */
    };

    //KeyCode represents the intention behind the key press. It depends on the button on the keyboard
    //being pressed, the active/pressed modifiers, selected language, keyboard type and probably other things.
    enum class KeyCode : uint32
    {
        UNKNOWN = 0x00000000u, /**< 0 */
        RETURN = 0x0000000du, /**< '\r' */
        ESCAPE = 0x0000001bu, /**< '\x1B' */
        BACKSPACE = 0x00000008u, /**< '\b' */
        TAB = 0x00000009u, /**< '\t' */
        SPACE = 0x00000020u, /**< ' ' */
        EXCLAIM = 0x00000021u, /**< '!' */
        DBLAPOSTROPHE = 0x00000022u, /**< '"' */
        HASH = 0x00000023u, /**< '#' */
        DOLLAR = 0x00000024u, /**< '$' */
        PERCENT = 0x00000025u, /**< '%' */
        AMPERSAND = 0x00000026u, /**< '&' */
        APOSTROPHE = 0x00000027u, /**< '\'' */
        LEFTPAREN = 0x00000028u, /**< '(' */
        RIGHTPAREN = 0x00000029u, /**< ')' */
        ASTERISK = 0x0000002au, /**< '*' */
        PLUS = 0x0000002bu, /**< '+' */
        COMMA = 0x0000002cu, /**< ',' */
        MINUS = 0x0000002du, /**< '-' */
        PERIOD = 0x0000002eu, /**< '.' */
        SLASH = 0x0000002fu, /**< '/' */
        NUM0 = 0x00000030u, /**< '0' */
        NUM1 = 0x00000031u, /**< '1' */
        NUM2 = 0x00000032u, /**< '2' */
        NUM3 = 0x00000033u, /**< '3' */
        NUM4 = 0x00000034u, /**< '4' */
        NUM5 = 0x00000035u, /**< '5' */
        NUM6 = 0x00000036u, /**< '6' */
        NUM7 = 0x00000037u, /**< '7' */
        NUM8 = 0x00000038u, /**< '8' */
        NUM9 = 0x00000039u, /**< '9' */
        COLON = 0x0000003au, /**< ':' */
        SEMICOLON = 0x0000003bu, /**< ';' */
        LESS = 0x0000003cu, /**< '<' */
        EQUALS = 0x0000003du, /**< '=' */
        GREATER = 0x0000003eu, /**< '>' */
        QUESTION = 0x0000003fu, /**< '?' */
        AT = 0x00000040u, /**< '@' */
        LEFTBRACKET = 0x0000005bu, /**< '[' */
        BACKSLASH = 0x0000005cu, /**< '\\' */
        RIGHTBRACKET = 0x0000005du, /**< ']' */
        CARET = 0x0000005eu, /**< '^' */
        UNDERSCORE = 0x0000005fu, /**< '_' */
        GRAVE = 0x00000060u, /**< '`' */
        A = 0x00000061u, /**< 'a' */
        B = 0x00000062u, /**< 'b' */
        C = 0x00000063u, /**< 'c' */
        D = 0x00000064u, /**< 'd' */
        E = 0x00000065u, /**< 'e' */
        F = 0x00000066u, /**< 'f' */
        G = 0x00000067u, /**< 'g' */
        H = 0x00000068u, /**< 'h' */
        I = 0x00000069u, /**< 'i' */
        J = 0x0000006au, /**< 'j' */
        K = 0x0000006bu, /**< 'k' */
        L = 0x0000006cu, /**< 'l' */
        M = 0x0000006du, /**< 'm' */
        N = 0x0000006eu, /**< 'n' */
        O = 0x0000006fu, /**< 'o' */
        P = 0x00000070u, /**< 'p' */
        Q = 0x00000071u, /**< 'q' */
        R = 0x00000072u, /**< 'r' */
        S = 0x00000073u, /**< 's' */
        T = 0x00000074u, /**< 't' */
        U = 0x00000075u, /**< 'u' */
        V = 0x00000076u, /**< 'v' */
        W = 0x00000077u, /**< 'w' */
        X = 0x00000078u, /**< 'x' */
        Y = 0x00000079u, /**< 'y' */
        Z = 0x0000007au, /**< 'z' */
        LEFTBRACE = 0x0000007bu, /**< '{' */
        PIPE = 0x0000007cu, /**< '|' */
        RIGHTBRACE = 0x0000007du, /**< '}' */
        TILDE = 0x0000007eu, /**< '~' */
        DELETE = 0x0000007fu, /**< '\x7F' */
        PLUSMINUS = 0x000000b1u, /**< '\xB1' */
        CAPSLOCK = 0x40000039u,
        F1 = 0x4000003au,
        F2 = 0x4000003bu,
        F3 = 0x4000003cu,
        F4 = 0x4000003du,
        F5 = 0x4000003eu,
        F6 = 0x4000003fu,
        F7 = 0x40000040u,
        F8 = 0x40000041u,
        F9 = 0x40000042u,
        F10 = 0x40000043u,
        F11 = 0x40000044u,
        F12 = 0x40000045u,
        PRINTSCREEN = 0x40000046u,
        SCROLLLOCK = 0x40000047u,
        PAUSE = 0x40000048u,
        INSERT = 0x40000049u,
        HOME = 0x4000004au,
        PAGEUP = 0x4000004bu,
        END = 0x4000004du,
        PAGEDOWN = 0x4000004eu,
        RIGHT = 0x4000004fu,
        LEFT = 0x40000050u,
        DOWN = 0x40000051u,
        UP = 0x40000052u,
        NUMLOCKCLEAR = 0x40000053u,
        KP_DIVIDE = 0x40000054u,
        KP_MULTIPLY = 0x40000055u,
        KP_MINUS = 0x40000056u,
        KP_PLUS = 0x40000057u,
        KP_ENTER = 0x40000058u,
        KP_1 = 0x40000059u,
        KP_2 = 0x4000005au,
        KP_3 = 0x4000005bu,
        KP_4 = 0x4000005cu,
        KP_5 = 0x4000005du,
        KP_6 = 0x4000005eu,
        KP_7 = 0x4000005fu,
        KP_8 = 0x40000060u,
        KP_9 = 0x40000061u,
        KP_0 = 0x40000062u,
        KP_PERIOD = 0x40000063u,
        APPLICATION = 0x40000065u,
        POWER = 0x40000066u,
        KP_EQUALS = 0x40000067u,
        F13 = 0x40000068u,
        F14 = 0x40000069u,
        F15 = 0x4000006au,
        F16 = 0x4000006bu,
        F17 = 0x4000006cu,
        F18 = 0x4000006du,
        F19 = 0x4000006eu,
        F20 = 0x4000006fu,
        F21 = 0x40000070u,
        F22 = 0x40000071u,
        F23 = 0x40000072u,
        F24 = 0x40000073u,
        EXECUTE = 0x40000074u,
        HELP = 0x40000075u,
        MENU = 0x40000076u,
        SELECT = 0x40000077u,
        STOP = 0x40000078u,
        AGAIN = 0x40000079u,
        UNDO = 0x4000007au,
        CUT = 0x4000007bu,
        COPY = 0x4000007cu,
        PASTE = 0x4000007du,
        FIND = 0x4000007eu,
        MUTE = 0x4000007fu,
        VOLUMEUP = 0x40000080u,
        VOLUMEDOWN = 0x40000081u,
        KP_COMMA = 0x40000085u,
        KP_EQUALSAS400 = 0x40000086u,
        ALTERASE = 0x40000099u,
        SYSREQ = 0x4000009au,
        CANCEL = 0x4000009bu,
        CLEAR = 0x4000009cu,
        PRIOR = 0x4000009du,
        RETURN2 = 0x4000009eu,
        SEPARATOR = 0x4000009fu,
        OUT = 0x400000a0u,
        OPER = 0x400000a1u,
        CLEARAGAIN = 0x400000a2u,
        CRSEL = 0x400000a3u,
        EXSEL = 0x400000a4u,
        KP_00 = 0x400000b0u,
        KP_000 = 0x400000b1u,
        THOUSANDSSEPARATOR = 0x400000b2u,
        DECIMALSEPARATOR = 0x400000b3u,
        CURRENCYUNIT = 0x400000b4u,
        CURRENCYSUBUNIT = 0x400000b5u,
        KP_LEFTPAREN = 0x400000b6u,
        KP_RIGHTPAREN = 0x400000b7u,
        KP_LEFTBRACE = 0x400000b8u,
        KP_RIGHTBRACE = 0x400000b9u,
        KP_TAB = 0x400000bau,
        KP_BACKSPACE = 0x400000bbu,
        KP_A = 0x400000bcu,
        KP_B = 0x400000bdu,
        KP_C = 0x400000beu,
        KP_D = 0x400000bfu,
        KP_E = 0x400000c0u,
        KP_F = 0x400000c1u,
        KP_XOR = 0x400000c2u,
        KP_POWER = 0x400000c3u,
        KP_PERCENT = 0x400000c4u,
        KP_LESS = 0x400000c5u,
        KP_GREATER = 0x400000c6u,
        KP_AMPERSAND = 0x400000c7u,
        KP_DBLAMPERSAND = 0x400000c8u,
        KP_VERTICALBAR = 0x400000c9u,
        KP_DBLVERTICALBAR = 0x400000cau,
        KP_COLON = 0x400000cbu,
        KP_HASH = 0x400000ccu,
        KP_SPACE = 0x400000cdu,
        KP_AT = 0x400000ceu,
        KP_EXCLAM = 0x400000cfu,
        KP_MEMSTORE = 0x400000d0u,
        KP_MEMRECALL = 0x400000d1u,
        KP_MEMCLEAR = 0x400000d2u,
        KP_MEMADD = 0x400000d3u,
        KP_MEMSUBTRACT = 0x400000d4u,
        KP_MEMMULTIPLY = 0x400000d5u,
        KP_MEMDIVIDE = 0x400000d6u,
        KP_PLUSMINUS = 0x400000d7u,
        KP_CLEAR = 0x400000d8u,
        KP_CLEARENTRY = 0x400000d9u,
        KP_BINARY = 0x400000dau,
        KP_OCTAL = 0x400000dbu,
        KP_DECIMAL = 0x400000dcu,
        KP_HEXADECIMAL = 0x400000ddu,
        LCTRL = 0x400000e0u,
        LSHIFT = 0x400000e1u,
        LALT = 0x400000e2u,
        LGUI = 0x400000e3u,
        RCTRL = 0x400000e4u,
        RSHIFT = 0x400000e5u,
        RALT = 0x400000e6u,
        RGUI = 0x400000e7u,
        MODE = 0x40000101u,
        SLEEP = 0x40000102u,
        WAKE = 0x40000103u,
        CHANNEL_INCREMENT = 0x40000104u,
        CHANNEL_DECREMENT = 0x40000105u,
        MEDIA_PLAY = 0x40000106u,
        MEDIA_PAUSE = 0x40000107u,
        MEDIA_RECORD = 0x40000108u,
        MEDIA_FAST_FORWARD = 0x40000109u,
        MEDIA_REWIND = 0x4000010au,
        MEDIA_NEXT_TRACK = 0x4000010bu,
        MEDIA_PREVIOUS_TRACK = 0x4000010cu,
        MEDIA_STOP = 0x4000010du,
        MEDIA_EJECT = 0x4000010eu,
        MEDIA_PLAY_PAUSE = 0x4000010fu,
        MEDIA_SELECT = 0x40000110u,
        AC_NEW = 0x40000111u,
        AC_OPEN = 0x40000112u,
        AC_CLOSE = 0x40000113u,
        AC_EXIT = 0x40000114u,
        AC_SAVE = 0x40000115u,
        AC_PRINT = 0x40000116u,
        AC_PROPERTIES = 0x40000117u,
        AC_SEARCH = 0x40000118u,
        AC_HOME = 0x40000119u,
        AC_BACK = 0x4000011au,
        AC_FORWARD = 0x4000011bu,
        AC_STOP = 0x4000011cu,
        AC_REFRESH = 0x4000011du,
        AC_BOOKMARKS = 0x4000011eu,
        SOFTLEFT = 0x4000011fu,
        SOFTRIGHT = 0x40000120u,
        CALL = 0x40000121u,
        ENDCALL = 0x40000122u,
        LEFT_TAB = 0x20000001u,
        LEVEL5_SHIFT = 0x20000002u,
        MULTI_KEY_COMPOSE = 0x20000003u,
        LMETA = 0x20000004u,
        RMETA = 0x20000005u,
        LHYPER = 0x20000006u,
        RHYPER = 0x20000007u,
    };

    //Modifiers that are currently active
    enum class KeyModifier
    {
        NONE = 0x0000u, /**< no modifier is applicable. */
        LSHIFT = 0x0001u, /**< the left Shift key is down. */
        RSHIFT = 0x0002u, /**< the right Shift key is down. */
        LEVEL5 = 0x0004u, /**< the Level 5 Shift key is down. */
        LCTRL = 0x0040u, /**< the left Ctrl (Control) key is down. */
        RCTRL = 0x0080u, /**< the right Ctrl (Control) key is down. */
        LALT = 0x0100u, /**< the left Alt key is down. */
        RALT = 0x0200u, /**< the right Alt key is down. */
        LGUI = 0x0400u, /**< the left GUI key (often the Windows key) is down. */
        RGUI = 0x0800u, /**< the right GUI key (often the Windows key) is down. */
        NUM = 0x1000u, /**< the Num Lock key (may be located on an extended keypad) is down. */
        CAPS = 0x2000u, /**< the Caps Lock key is down. */
        MODE = 0x4000u, /**< the !AltGr key is down. */
        SCROLL = 0x8000u, /**< the Scroll Lock key is down. */
        CTRL = (LCTRL | RCTRL),   /**< Any Ctrl key is down. */
        SHIFT = (LSHIFT | RSHIFT), /**< Any Shift key is down. */
        ALT = (LALT | RALT),     /**< Any Alt key is down. */
        GUI = (LGUI | RGUI)     /**< Any GUI key is down. */
    };
    ENUM_CLASS_BITWISE_OPERATIONS(KeyModifier)

    enum class MouseButton : uint8
    {
        Left,
        Right,
        Middle,
        Other,
        COUNT
    };
    enum class ScrollDirection
    {
        Normal,
        Flipped
    };

    using KeyboardID = uint32;
    using MouseID = uint32;

    struct KeyDownEvent
    {
        Window* window;
        uint64 timeNS;
        KeyboardID keyboardID;

        Key key;
        KeyModifier modifier;
        KeyCode keyCode;

        bool repeat;
    };
    struct KeyUpEvent
    {
        Window* window;
        uint64 timeNS;
        KeyboardID keyboardID;

        Key key;
        KeyModifier modifier;
        KeyCode keyCode;
    };
    struct TextInputEvent
    {
        Window* window;
        uint64 timeNS;

        u8String string;
    };
    struct MouseButtonDownEvent
    {
        Window* window;
        uint64 timeNS;
        MouseID mouseID;

        MouseButton button;
        uint combo; //number of consecutive clicks
        Vec2f pos; //position in window coordinates
    };
    struct MouseButtonUpEvent
    {
        Window* window;
        uint64 timeNS;
        MouseID mouseID;

        MouseButton button;
        uint combo; //number of consecutive clicks
        Vec2f pos; //position in window coordinates
    };
    struct MouseMotionEvent
    {
        Window* window;
        uint64 timeNS;
        MouseID mouseID;

        Vec2f delta;
        Vec2f pos; //position in window coordinates
    };
    struct MouseScrollEvent
    {
        Window* window;
        uint64 timeNS;
        MouseID mouseID;

        Vec2f amount;
        ScrollDirection direction;
        Vec2f pos; //position in window coordinates
    };

    struct KeyFrameState
    {
        //This value is true if the key was pressed at least once between two Input::Update() calls
        bool pressed;
        //This value is true if the key was released at least once between two Input::Update() calls
        bool released;
        //This value is true if the key was down at one point between two Input::Update() calls
        bool down;
        //This value is true if the key was up at one point between two Input::Update() calls
        bool up;
    };
    struct MouseButtonFrameState
    {
        uint8 combo;
        //This value is true if the key was pressed at least once between two Input::Update() calls
        bool pressed;
        //This value is true if the key was released at least once between two Input::Update() calls
        bool released;
        //This value is true if the key was down at one point between two Input::Update() calls
        bool down;
        //This value is true if the key was up at one point between two Input::Update() calls
        bool up;
    };

    enum class CursorType
    {
        Default,
        Text,
        Wait,
        Crosshair,
        Progress,
        NotAllowed,
        Move,
        Pointer,
        ResizeNWSE,
        ResizeNESW,
        ResizeNW,
        ResizeNS,
        ResizeNE,
        ResizeSE,
        ResizeSW,
        ResizeEW,
        ResizeN,
        ResizeW,
        ResizeS,
        ResizeE,
        CursorCount
    };

    BLAZE_API KeyCode GetKeyCode(Key key, KeyModifier modifier);
    BLAZE_API StringView GetKeyName(KeyCode keyCode);

    /*
        Returns key state between two Input::Update() calls. By using this function an application wont be
        able to register multiple key presses or releases between two Input::Update() calls, they can
        register only if they happened at least once. For full control over key presses and releases use
        event handling to handle key states.

        \param key - the key whose state to get
        \return A KeyFrameState structure with information about the state of the key between two Input::Update()
        calls
    */
    BLAZE_API KeyFrameState GetKeyFrameState(Key key);
    /*
        Returns mouse button state between two Input::Update() calls. By using this function an application
        wont be able to register multiple key presses or releases between two Input::Update() calls, they
        can register only if they happened at least once. For full control over mouse button presses and
        releases use event handling.

        \param button - the mouse button whose state to retrieve
        \return A MouseButtonFrameState structure with information about the state of the key between two
        Input::Update() calls
    */
    BLAZE_API MouseButtonFrameState GetMouseButtonFrameState(MouseButton button);

    BLAZE_API void SetCursorType(CursorType type);
    BLAZE_API void ShowCursor(bool show);
}