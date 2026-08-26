//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_WAYLAND_CONVERTER_HPP
#define CHARTIS_WAYLAND_CONVERTER_HPP

#include <cstdint>
#include <linux/input-event-codes.h>
#include "../include/platypus/UserInput.hpp"

namespace chartis::platypus {
    //////////////////////
    // INPUT CONVERTERS //
    //////////////////////

    static Key waylandKeyToMavisKey(uint32_t key) {
        if (key == KEY_A) return Key::A;
        if (key == KEY_B) return Key::B;
        if (key == KEY_C) return Key::C;
        if (key == KEY_D) return Key::D;
        if (key == KEY_E) return Key::E;
        if (key == KEY_F) return Key::F;
        if (key == KEY_G) return Key::G;
        if (key == KEY_H) return Key::H;
        if (key == KEY_I) return Key::I;
        if (key == KEY_J) return Key::J;
        if (key == KEY_K) return Key::K;
        if (key == KEY_L) return Key::L;
        if (key == KEY_M) return Key::M;
        if (key == KEY_N) return Key::N;
        if (key == KEY_O) return Key::O;
        if (key == KEY_P) return Key::P;
        if (key == KEY_Q) return Key::Q;
        if (key == KEY_R) return Key::R;
        if (key == KEY_S) return Key::S;
        if (key == KEY_T) return Key::T;
        if (key == KEY_U) return Key::U;
        if (key == KEY_V) return Key::V;
        if (key == KEY_W) return Key::W;
        if (key == KEY_X) return Key::X;
        if (key == KEY_Y) return Key::Y;
        if (key == KEY_Z) return Key::Z;

        if (key == KEY_LEFTSHIFT) return Key::LEFT_SHIFT;
        if (key == KEY_LEFTCTRL) return Key::LEFT_CONTROL;
        if (key == KEY_LEFTMETA) return Key::LEFT_SUPER;
        if (key == KEY_LEFTALT) return Key::LEFT_ALT;

        if (key == KEY_RIGHTSHIFT) return Key::RIGHT_SHIFT;
        if (key == KEY_RIGHTCTRL) return Key::RIGHT_CONTROL;
        if (key == KEY_RIGHTMETA) return Key::RIGHT_SUPER;
        if (key == KEY_RIGHTALT) return Key::RIGHT_ALT;

        if (key == KEY_F1) return Key::F1;
        if (key == KEY_F2) return Key::F2;
        if (key == KEY_F3) return Key::F3;
        if (key == KEY_F4) return Key::F4;
        if (key == KEY_F5) return Key::F5;
        if (key == KEY_F6) return Key::F6;
        if (key == KEY_F7) return Key::F7;
        if (key == KEY_F8) return Key::F8;
        if (key == KEY_F9) return Key::F9;
        if (key == KEY_F10) return Key::F10;
        if (key == KEY_F11) return Key::F11;
        if (key == KEY_F12) return Key::F12;


        return Key::NONE;
    }
}

#endif //CHARTIS_WAYLAND_CONVERTER_HPP
