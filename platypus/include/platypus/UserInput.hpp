////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_USERINPUT_HPP
#define CHARTIS_USERINPUT_HPP

#include <functional>

namespace chartis {
    enum class MouseButton {
        LEFT, MIDDLE, RIGHT
    };

    enum Mods {
        NONE = 0,
        SHIFT = 1 << 1,
        CONTROL = 1 << 2,
        ALT = 1 << 3
    };

    enum class Key {
        NONE,
        ESC, TAB, CAPS_LOCK, SPACE, BACK_SPACE, ENTER,
        LEFT_SHIFT, LEFT_CONTROL, LEFT_SUPER, LEFT_ALT,
        RIGHT_SHIFT, RIGHT_CONTROL, RIGHT_SUPER, RIGHT_ALT,
        UP, DOWN, LEFT, RIGHT,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
        NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
    };

    enum class Action {
        PRESS, RELEASE
    };

    typedef std::function<void(double x, double y)>                           pointer_enter_callback;
    typedef std::function<void()>                                             pointer_leave_callback;
    typedef std::function<void(double x, double y)>                           pointer_move_callback;
    typedef std::function<void(MouseButton button, Action action, Mods mods)> pointer_button_callback;
    typedef std::function<void(double xOffset, double yOffset)>               pointer_scroll_callback;
    typedef std::function<void(Key key, Action action, Mods mods)>            key_callback;
    typedef std::function<void(int width, int height)>                        window_resize_callback;

    enum class Cursor {
        ARROW,
        ROTATE,
        CROSSHAIR,
        SCALE_HORIZONTAL,
        SCALE_VERTICAL,
        SHEAR_HORIZONTAL,
        SHEAR_VERTICAL
    };
}

#endif //CHARTIS_USERINPUT_HPP
