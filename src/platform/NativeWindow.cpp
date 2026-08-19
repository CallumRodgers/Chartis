//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "NativeWindow.hpp"

#include "Wayland.hpp"
#include "X11.hpp"
#include "Windows.hpp"
#include "Apple.hpp"

using namespace mavis::platform;

#if defined(__linux__) || defined(__FreeBSD__)
///////////////////////////////////////////////
// BEGIN LINUX PLATFORM CODE (WAYLAND & X11) //
///////////////////////////////////////////////
#include <wayland-client-core.h>

bool isWayland = false;

void isWaylandCheck() {
    wl_display* display = wl_display_connect(nullptr);
    if (display) {
        wl_display_disconnect(display);
        isWayland = true;
    }
}

NativeWindow::NativeWindow() {
    isWaylandCheck();
    if (isWayland) {
        mWayland = std::make_unique<Wayland>();
        mPlatformInfo = {
            .platform = amora::Platform::Wayland,
            .windowHandle = mWayland->getSurface(),
            .displayHandle = mWayland->getDisplay()
        };
    } else {
        mX11 = std::make_unique<X11>();
    }
}

NativeWindow::~NativeWindow() = default;

void NativeWindow::setPointerEnterCallback(const pointer_enter_callback& callback) const {
    mWayland->mPointerEnterCallback = callback;
}

void NativeWindow::setPointerLeaveCallback(const pointer_leave_callback& callback) const {
    mWayland->mPointerLeaveCallback = callback;
}

void NativeWindow::setPointerMoveCallback(const pointer_move_callback& callback) const {
    mWayland->mPointerMoveCallback = callback;
}

void NativeWindow::setPointerButtonCallback(const pointer_button_callback& callback) const {
    mWayland->mPointerButtonCallback = callback;
}

void NativeWindow::setPointerScrollCallback(const pointer_scroll_callback& callback) const {
    mWayland->mPointerScrollCallback = callback;
}

void NativeWindow::setKeyCallback(const key_callback& callback) const {
    mWayland->mKeyCallback = callback;
}

void NativeWindow::setWindowResizeCallback(const window_resize_callback& callback) const {
    mWayland->mWindowResizeCallback = callback;
}

void NativeWindow::showNativeWindowMenu() const { mWayland->showNativeWindowMenu(); }
void NativeWindow::drag() const { mWayland->drag(); }
void NativeWindow::minimise() const { mWayland->minimise(); }
void NativeWindow::close() const { mWayland->m_running = false; }
bool NativeWindow::isClosing() const { return mWayland->shouldClose(); }
void NativeWindow::pollEvents() const { mWayland->pollEvents(); }

void NativeWindow::toggleMaximise() const { mWayland->toggleMaximise(); }
bool NativeWindow::isMaximised() const { return mWayland->m_maximised; }

void NativeWindow::toggleFullscreen() const { mWayland->toggleFullscreen(); }
bool NativeWindow::isFullscreen() const { return mWayland->m_fullscreen; }

std::pair<uint32_t, uint32_t> NativeWindow::getWindowSize() const {
    return {mWayland->m_width, mWayland->m_height};
}

void NativeWindow::setCursor(Cursor cursor) const { mWayland->setCursor(cursor); }

std::pair<double, double> NativeWindow::getPointerPos() const {
    return {5.0f, 50.f};
}

/////////////////////////////
// END LINUX PLATFORM CODE //
/////////////////////////////
#elif defined(__WIN32)
/////////////////////////////////
// BEGIN WINDOWS PLATFORM CODE //
/////////////////////////////////



#endif