////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_WAYLAND_HPP
#define CHARTIS_WAYLAND_HPP

#include <string>

#include "../include/platypus/UserInput.hpp"
#include <GLFW/glfw3.h>

////////////////////////////////
// BEGIN FORWARD DECLARATIONS //
struct wl_display;
struct wl_registry;
struct wl_shm;
struct wl_compositor;
struct wl_surface;
struct wl_seat;
struct wl_pointer;
struct wl_keyboard;

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;
// END FORWARD DECLARATIONS //
//////////////////////////////

namespace chartis::platypus {
    /// Wayland-specific platform code.
    class Wayland {
    public:
        Wayland();

        bool shouldClose() const;

        void handleGlobal(wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
        void handleGlobalRemove(wl_registry* registry, uint32_t name);
        void handlePointerListener();
        void handleKeyboardListener();

        void drag() const;
        void showNativeWindowMenu() const;
        void minimise() const;
        void toggleMaximise() const;
        void toggleFullscreen() const;
        void pollEvents() const;

        void setCursor(Cursor cursor);

        wl_display* getDisplay() const { return m_display; }
        wl_surface* getSurface() const { return m_surface; }

        bool m_maximised, m_maximised_pending;
        bool m_fullscreen, m_fullscreen_pending;

        bool m_configured = false;
        bool m_running = false;

        //////////////////////////
        // USER INPUT CALLBACKS //
        //////////////////////////

        pointer_enter_callback mPointerEnterCallback {nullptr};
        pointer_leave_callback mPointerLeaveCallback {nullptr};
        pointer_move_callback mPointerMoveCallback {nullptr};
        pointer_button_callback mPointerButtonCallback {nullptr};
        pointer_scroll_callback mPointerScrollCallback {nullptr};
        key_callback mKeyCallback {nullptr};
        window_resize_callback mWindowResizeCallback {nullptr};

        uint32_t m_width, m_height;
        uint32_t m_pending_width, m_pending_height;
        uint32_t m_max_width, m_max_height;

        uint32_t m_last_button_serial = 0;
        uint32_t m_initial_configure_serial = 0;
        uint32_t m_last_enter_serial = 0;
        int32_t m_pointer_x, m_pointer_y;

        wl_display*    m_display    = nullptr;
        wl_registry*   m_registry   = nullptr;
        wl_shm*        m_shm        = nullptr;
        wl_compositor* m_compositor = nullptr;
        wl_surface*    m_surface    = nullptr;
        wl_seat*       m_seat       = nullptr;
        wl_pointer*    m_pointer    = nullptr;
        wl_keyboard*   m_keyboard   = nullptr;

        xdg_wm_base*  m_xdg_wm_base  = nullptr;
        xdg_surface*  m_xdg_surface  = nullptr;
        xdg_toplevel* m_xdg_toplevel = nullptr;

        zxdg_decoration_manager_v1*  m_xdg_decoration_manager = nullptr;
        zxdg_toplevel_decoration_v1* m_xdg_decoration         = nullptr;

    private:
        struct CursorImage {
            wl_surface* surface;
            int32_t hotspotX;
            int32_t hotspotY;
        };

        void createCursors();
        void createCursor(Cursor cursor, const std::string& filePath, int hotspotX, int hotspotY);

        std::unordered_map<Cursor, CursorImage> mCursors;
    };
}

#endif //CHARTIS_WAYLAND_HPP
