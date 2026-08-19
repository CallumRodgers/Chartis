//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_KEY_CONVERTER_HPP
#define OPENMAVIS_KEY_CONVERTER_HPP

#include <imgui.h>
#include "platform/UserInput.hpp"

namespace mavis {
    static ImGuiKey keyToImGuiKey(Key key) {
        switch (key) {
            case Key::TAB: return ImGuiKey_Tab;
            case Key::LEFT: return ImGuiKey_LeftArrow;
            case Key::RIGHT: return ImGuiKey_RightArrow;
            case Key::UP: return ImGuiKey_UpArrow;
            case Key::DOWN: return ImGuiKey_DownArrow;
            //case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
            //case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
            //case GLFW_KEY_HOME: return ImGuiKey_Home;
            //case GLFW_KEY_END: return ImGuiKey_End;
            //case GLFW_KEY_INSERT: return ImGuiKey_Insert;
            //case GLFW_KEY_DELETE: return ImGuiKey_Delete;
            case Key::BACK_SPACE: return ImGuiKey_Backspace;
            case Key::SPACE: return ImGuiKey_Space;
            case Key::ENTER: return ImGuiKey_Enter;
            case Key::ESC: return ImGuiKey_Escape;
            //case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
            //case GLFW_KEY_COMMA: return ImGuiKey_Comma;
            //case GLFW_KEY_MINUS: return ImGuiKey_Minus;
            //case GLFW_KEY_PERIOD: return ImGuiKey_Period;
            //case GLFW_KEY_SLASH: return ImGuiKey_Slash;
            //case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
            //case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
            //case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
            //case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
            //case GLFW_KEY_WORLD_1: return ImGuiKey_Oem102;
            //case GLFW_KEY_WORLD_2: return ImGuiKey_Oem102;
            //case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
            //case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
            case Key::CAPS_LOCK: return ImGuiKey_CapsLock;
            //case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
            //case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
            //case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
            //case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
            case Key::NP0: return ImGuiKey_Keypad0;
            case Key::NP1: return ImGuiKey_Keypad1;
            case Key::NP2: return ImGuiKey_Keypad2;
            case Key::NP3: return ImGuiKey_Keypad3;
            case Key::NP4: return ImGuiKey_Keypad4;
            case Key::NP5: return ImGuiKey_Keypad5;
            case Key::NP6: return ImGuiKey_Keypad6;
            case Key::NP7: return ImGuiKey_Keypad7;
            case Key::NP8: return ImGuiKey_Keypad8;
            case Key::NP9: return ImGuiKey_Keypad9;
            //case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
            //case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
            //case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
            //case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
            //case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
            //case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
            //case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
            case Key::LEFT_SHIFT: return ImGuiKey_LeftShift;
            case Key::LEFT_CONTROL: return ImGuiKey_LeftCtrl;
            case Key::LEFT_ALT: return ImGuiKey_LeftAlt;
            case Key::LEFT_SUPER: return ImGuiKey_LeftSuper;
            case Key::RIGHT_SHIFT: return ImGuiKey_RightShift;
            case Key::RIGHT_CONTROL: return ImGuiKey_RightCtrl;
            case Key::RIGHT_ALT: return ImGuiKey_RightAlt;
            case Key::RIGHT_SUPER: return ImGuiKey_RightSuper;
            //case GLFW_KEY_MENU: return ImGuiKey_Menu;
            case Key::N0: return ImGuiKey_0;
            case Key::N1: return ImGuiKey_1;
            case Key::N2: return ImGuiKey_2;
            case Key::N3: return ImGuiKey_3;
            case Key::N4: return ImGuiKey_4;
            case Key::N5: return ImGuiKey_5;
            case Key::N6: return ImGuiKey_6;
            case Key::N7: return ImGuiKey_7;
            case Key::N8: return ImGuiKey_8;
            case Key::N9: return ImGuiKey_9;
            case Key::A: return ImGuiKey_A;
            case Key::B: return ImGuiKey_B;
            case Key::C: return ImGuiKey_C;
            case Key::D: return ImGuiKey_D;
            case Key::E: return ImGuiKey_E;
            case Key::F: return ImGuiKey_F;
            case Key::G: return ImGuiKey_G;
            case Key::H: return ImGuiKey_H;
            case Key::I: return ImGuiKey_I;
            case Key::J: return ImGuiKey_J;
            case Key::K: return ImGuiKey_K;
            case Key::L: return ImGuiKey_L;
            case Key::M: return ImGuiKey_M;
            case Key::N: return ImGuiKey_N;
            case Key::O: return ImGuiKey_O;
            case Key::P: return ImGuiKey_P;
            case Key::Q: return ImGuiKey_Q;
            case Key::R: return ImGuiKey_R;
            case Key::S: return ImGuiKey_S;
            case Key::T: return ImGuiKey_T;
            case Key::U: return ImGuiKey_U;
            case Key::V: return ImGuiKey_V;
            case Key::W: return ImGuiKey_W;
            case Key::X: return ImGuiKey_X;
            case Key::Y: return ImGuiKey_Y;
            case Key::Z: return ImGuiKey_Z;
            case Key::F1: return ImGuiKey_F1;
            case Key::F2: return ImGuiKey_F2;
            case Key::F3: return ImGuiKey_F3;
            case Key::F4: return ImGuiKey_F4;
            case Key::F5: return ImGuiKey_F5;
            case Key::F6: return ImGuiKey_F6;
            case Key::F7: return ImGuiKey_F7;
            case Key::F8: return ImGuiKey_F8;
            case Key::F9: return ImGuiKey_F9;
            case Key::F10: return ImGuiKey_F10;
            case Key::F11: return ImGuiKey_F11;
            case Key::F12: return ImGuiKey_F12;
            //case Key::F13: return ImGuiKey_F13;
            //case Key::F14: return ImGuiKey_F14;
            //case Key::F15: return ImGuiKey_F15;
            //case Key::F16: return ImGuiKey_F16;
            //case Key::F17: return ImGuiKey_F17;
            //case Key::F18: return ImGuiKey_F18;
            //case Key::F19: return ImGuiKey_F19;
            //case Key::F20: return ImGuiKey_F20;
            //case Key::F21: return ImGuiKey_F21;
            //case Key::F22: return ImGuiKey_F22;
            //case Key::F23: return ImGuiKey_F23;
            //case Key::F24: return ImGuiKey_F24;
            default: return ImGuiKey_None;
        }
    }
}

#endif //OPENMAVIS_KEY_CONVERTER_HPP
