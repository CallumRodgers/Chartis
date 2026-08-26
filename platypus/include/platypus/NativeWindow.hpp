//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_WINDOWPLATFORM_HPP
#define CHARTIS_WINDOWPLATFORM_HPP

#include <cstdint>
#include <memory>

#include "Platform.hpp"
#include "UserInput.hpp"

/**
 * The <code>platform</code> namespace encapsulates (most of) the platform code within Chartis;
 * at least the code that deals with window creation and user interaction.
 * The AMORA Engine also has a single platform-dependent class implementation, for the RenderSurface.
 * Since Vulkan is isolated in AMORA, the creation of a Vulkan surface from the platform window/surface
 * can't be done here.
 */
namespace chartis::platypus {
    ////////////////////////////////
    // BEGIN FORWARD DECLARATIONS //
    class Wayland;
    class X11;
    class Windows;
    class Apple;
    // END FORWARD DECLARATIONS   //
    ////////////////////////////////

    class NativeWindow {
    public:
        NativeWindow();
        ~NativeWindow();

        [[nodiscard]] const NativePlatformInfo& getNativePlatformInfo() const { return mPlatformInfo; }

        /////////////////////////////
        // WINDOW USER INTERACTION //
        /////////////////////////////

        void drag() const;
        void showNativeWindowMenu() const;
        void minimise() const;
        void toggleMaximise() const;
        void toggleFullscreen() const;

        bool isMaximised() const;
        bool isFullscreen() const;

        std::pair<double, double> getPointerPos() const;
        [[nodiscard]] std::pair<uint32_t, uint32_t> getWindowSize() const;
        void setCursor(Cursor cursor) const;

        ////////////////////////////////
        // WINDOW LIFECYCLE FUNCTIONS //
        ////////////////////////////////

        void close() const;
        [[nodiscard]] bool isClosing() const;
        void pollEvents() const;

        ////////////////////////
        // CALLBACK FUNCTIONS //
        ////////////////////////

        void setPointerEnterCallback(const pointer_enter_callback& callback) const;
        void setPointerLeaveCallback(const pointer_leave_callback& callback) const;
        void setPointerMoveCallback(const pointer_move_callback& callback) const;
        void setPointerButtonCallback(const pointer_button_callback& callback) const;
        void setPointerScrollCallback(const pointer_scroll_callback& callback) const;
        void setKeyCallback(const key_callback& callback) const;
        void setWindowResizeCallback(const window_resize_callback& callback) const;

    private:
        NativePlatformInfo mPlatformInfo;

        std::unique_ptr<Wayland> mWayland;
        std::unique_ptr<X11> mX11;
        std::unique_ptr<Windows> mWindows;
        std::unique_ptr<Apple> mApple;

        uint32_t mLastWindowX = 0;
        uint32_t mLastWindowY = 0;
        uint32_t mLastWindowWidth = 0;
        uint32_t mLastWindowHeight = 0;
    };
}

#endif //CHARTIS_WINDOWPLATFORM_HPP
