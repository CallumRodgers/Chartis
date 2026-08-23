//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "Wayland.hpp"

#include "ChartisApplication.hpp"

#include <cstring>
#include <stdexcept>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <xdg-decoration-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#include <poll.h>

#include "file_utils.hpp"
#include "wayland_converter.hpp"

using namespace chartis;
using namespace ::platform;

//////////////////////////////
// STATIC UTILITY FUNCTIONS //
//////////////////////////////

static wl_buffer* createBuffer(wl_shm* sharedMemory, const unsigned char* pixels, int32_t width, int32_t height) {
    const int32_t stride = width * 4;
    const int32_t size = stride * height;

    const int fd = memfd_create("cursor-shm", MFD_CLOEXEC);
    if (fd < 0) return nullptr;
    ftruncate(fd, size);

    // Copying pixels to the Wayland shared memory.
    void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memcpy(data, pixels, size);
    munmap(data, size);

    wl_shm_pool* pool = wl_shm_create_pool(sharedMemory, fd, size);
    wl_buffer* buffer = wl_shm_pool_create_buffer(
        pool,
        0,
        width,
        height,
        stride,
        WL_SHM_FORMAT_ARGB8888
    );

    wl_shm_pool_destroy(pool);
    close(fd);

    return buffer;
}

//////////////////////
// STATIC CALLBACKS //
//////////////////////

static void seat_handle_capabilities(void *data, wl_seat *seat, uint32_t capabilities) {
    auto* wayland = static_cast<Wayland*>(data);
    // If the wl_seat has the pointer capability, start listening to pointer events.
    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        wayland->m_pointer = wl_seat_get_pointer(seat);
        wayland->handlePointerListener();
    }
    // If the wl_seat has the keyboard capability, start listening to keyboard events.
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        wayland->m_keyboard = wl_seat_get_keyboard(seat);
        wayland->handleKeyboardListener();
    }
}

static void seat_handle_name(void* data, struct wl_seat* wl_seat, const char* name) {

}

static void handle_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    auto* wayland = static_cast<Wayland*>(data);
    if (wayland->m_configured) {
        xdg_surface_ack_configure(xdg_surface, serial);

        if (wayland->m_width != wayland->m_pending_width || wayland->m_pending_height != wayland->m_height) {
            wayland->m_width = wayland->m_pending_width;
            wayland->m_height = wayland->m_pending_height;
            wayland->mWindowResizeCallback(wayland->m_width, wayland->m_height);
        }

        if (wayland->m_maximised != wayland->m_maximised_pending) {
            wayland->m_maximised = wayland->m_maximised_pending;
            // TODO: Maximised callback.
        }

        if (wayland->m_fullscreen != wayland->m_fullscreen_pending) {
            wayland->m_fullscreen = wayland->m_fullscreen_pending;
            // TODO: Fullscreen callback.
        }

        // Commit is automatically called by Vulkan after presentation.
    } else {
        wayland->m_initial_configure_serial = serial;
        wayland->m_configured = true;

        wayland->m_width = wayland->m_pending_width;
        wayland->m_height = wayland->m_pending_height;
        wayland->m_maximised = wayland->m_maximised;
        wayland->m_fullscreen = wayland->m_fullscreen;
    }
}

static void handle_toplevel_configure(void* data, xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, wl_array* states) {
    auto* wayland = static_cast<Wayland*>(data);

    if (width > 0 && height > 0) {
        wayland->m_pending_width = width;
        wayland->m_pending_height = height;
    } else {
        wayland->m_pending_width = static_cast<uint32_t>(0.8 * wayland->m_max_width);
        wayland->m_pending_height = static_cast<uint32_t>(0.8 * wayland->m_max_height);
    }

    bool maximised = false;
    bool fullscreen = false;


    auto* state_ptr = static_cast<const uint32_t*>(states->data);
    const size_t count = states->size / sizeof(uint32_t);

    for (size_t i = 0; i < count; ++i) {
        const uint32_t state = state_ptr[i];

        if (state == XDG_TOPLEVEL_STATE_MAXIMIZED) {
            maximised = true;
        } else if (state == XDG_TOPLEVEL_STATE_FULLSCREEN) {
            fullscreen = true;
        }
    }

    wayland->m_maximised_pending = maximised;
    wayland->m_fullscreen_pending = fullscreen;
}

static void handle_toplevel_close(void* data, struct xdg_toplevel* xdg_toplevel) {
    auto* wayland = static_cast<Wayland*>(data);
    wayland->m_running = false;
}

//////////////////////
// STATIC LISTENERS //
//////////////////////

static constexpr xdg_wm_base_listener wm_base_listener = {
    .ping = [](void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial) {
        // The compositor will send us a ping event to check that we're responsive.
        // We need to send back a pong request immediately.
        xdg_wm_base_pong(xdg_wm_base, serial);
    }
};

static constexpr wl_registry_listener registry_listener = {
    .global = [](void *data, wl_registry *registry, uint32_t name, const char* interface, uint32_t version) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->handleGlobal(registry, name, interface, version);
    },
    .global_remove = [](void *data, wl_registry *registry, uint32_t name) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->handleGlobalRemove(registry, name);
    }
};

static constexpr wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_handle_name
};

static constexpr wl_pointer_listener pointer_listener = {
    .enter = [](void* data, struct wl_pointer* wl_pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->m_last_enter_serial = serial;
        wayland->mPointerEnterCallback(wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
    },
    .leave = [](void* data, struct wl_pointer* wl_pointer, uint32_t serial, struct wl_surface* surface) {
        static_cast<Wayland*>(data)->mPointerLeaveCallback();
    },
    .motion = [](void* data, struct wl_pointer* wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->m_pointer_x = wl_fixed_to_int(surface_x);
        wayland->m_pointer_y = wl_fixed_to_int(surface_y);
        wayland->mPointerMoveCallback(wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
    },
    .button = [](void* data, struct wl_pointer* wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->m_last_button_serial = serial;
        MouseButton mouseButton;
        switch (button) {
            default:
            case BTN_LEFT: { mouseButton = MouseButton::LEFT; break; }
            case BTN_MIDDLE: { mouseButton = MouseButton::MIDDLE; break; }
            case BTN_RIGHT: { mouseButton = MouseButton::RIGHT; break; }
        }
        Action action;
        switch (state) {
            default:
            case WL_POINTER_BUTTON_STATE_PRESSED: action = Action::PRESS; break;
            case WL_POINTER_BUTTON_STATE_RELEASED: action = Action::RELEASE; break;
        }
        wayland->mPointerButtonCallback(mouseButton, action, NONE);
    },
    .axis = [](void* data, struct wl_pointer* wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
        auto* wayland = static_cast<Wayland*>(data);
        if (axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
            wayland->mPointerScrollCallback(wl_fixed_to_double(-value) / 10.0, 0.0);
        } else if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
            wayland->mPointerScrollCallback(0.0, wl_fixed_to_double(-value) / 10.0);
        }
    },
    .frame = [](void* data, struct wl_pointer* wl_pointer) {},
    .axis_source = [](void* data, struct wl_pointer* wl_pointer, uint32_t axis_source) {},
    .axis_stop = [](void* data, struct wl_pointer* wl_pointer, uint32_t time, uint32_t axis) {},
    .axis_discrete = [](void* data, struct wl_pointer* wl_pointer, uint32_t axis, int32_t discrete) {},
    .axis_value120 = [](void* data, struct wl_pointer* wl_pointer, uint32_t axis, int32_t value120) {},
    .axis_relative_direction = [](void* data, struct wl_pointer* wl_pointer, uint32_t axis, uint32_t direction) {}
};

static constexpr wl_keyboard_listener keyboard_listener = {
    .keymap = [](void* data, struct wl_keyboard* wl_keyboard, uint32_t format, int32_t fd, uint32_t size) {},
    .enter = [](void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {},
    .leave = [](void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, struct wl_surface* surface) {},
    .key = [](void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
        Action action;
        switch (state) {
            default:
            case WL_KEYBOARD_KEY_STATE_PRESSED: action = Action::PRESS; break;
            case WL_KEYBOARD_KEY_STATE_RELEASED: action = Action::RELEASE; break;
        }
        static_cast<Wayland*>(data)->mKeyCallback(waylandKeyToMavisKey(key), action, static_cast<Mods>(0));
    },
    .modifiers = [](void* data, struct wl_keyboard* wl_keyboard, uint32_t serial, uint32_t mods_depressed,
        uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {},
    .repeat_info = [](void* data, struct wl_keyboard* wl_keyboard, int32_t rate, int32_t delay) {}
};

static constexpr xdg_surface_listener surface_listener = {
    .configure = handle_surface_configure,
};

static constexpr  xdg_toplevel_listener toplevel_listener = {
    .configure = handle_toplevel_configure,
    .close = handle_toplevel_close,
    .configure_bounds = [](void* data, struct xdg_toplevel* xdg_toplevel, int32_t width, int32_t height) {
        auto* wayland = static_cast<Wayland*>(data);
        wayland->m_max_width = width;
        wayland->m_max_height = height;
    },
    .wm_capabilities = [](void* data, struct xdg_toplevel* xdg_toplevel, struct wl_array* capabilities) {  }
};

//////////////////////////////////
// CLASS HANDLE IMPLEMENTATIONS //
//////////////////////////////////

void Wayland::handleGlobal(wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    if (strcmp(interface, wl_shm_interface.name) == 0) {
        m_shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, version));
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        m_seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, version));
        wl_seat_add_listener(m_seat, &seat_listener, this);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        m_compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        m_xdg_wm_base = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
        xdg_wm_base_add_listener(m_xdg_wm_base, &wm_base_listener, this);
    } else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
        m_xdg_decoration_manager = static_cast<zxdg_decoration_manager_v1*>(
            wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, version)
        );
    }
}

void Wayland::handleGlobalRemove(wl_registry* registry, uint32_t name) {

}

void Wayland::handlePointerListener() {
    wl_pointer_add_listener(m_pointer, &pointer_listener, this);
}

void Wayland::handleKeyboardListener() {
    wl_keyboard_add_listener(m_keyboard, &keyboard_listener, this);
}

/////////////////////////////////
// OTHER CLASS IMPLEMENTATIONS //
/////////////////////////////////

Wayland::Wayland() {
    CHARTIS_INFO("Starting Wayland window creation.");
    // Connecting to Wayland compositor.
    m_display = wl_display_connect(nullptr);
    if (m_display == nullptr) {
        throw std::runtime_error("Wayland: Failed to connect to display");
    }

    // Getting registry and globals.
    m_registry = wl_display_get_registry(m_display);
    wl_registry_add_listener(m_registry, &registry_listener, this);
    if (wl_display_roundtrip(m_display) == -1) {
        throw std::runtime_error("Wayland: Failed to wait for roundtrip");
    }

    if (m_compositor == nullptr || m_xdg_wm_base == nullptr) {
        throw std::runtime_error("Wayland: No compositor or xdg_wm_base support");
    }

    // Creating surfaces, both Wayland and XDG.
    m_surface = wl_compositor_create_surface(m_compositor);
    m_xdg_surface  = xdg_wm_base_get_xdg_surface(m_xdg_wm_base, m_surface);
    m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);

    xdg_toplevel_set_title(m_xdg_toplevel, "Chartis");

    if (m_xdg_decoration_manager != nullptr) {
        m_xdg_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(m_xdg_decoration_manager, m_xdg_toplevel);
        zxdg_toplevel_decoration_v1_set_mode(m_xdg_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
    }

    // Attaching listeners to XDG objects.
    xdg_surface_add_listener(m_xdg_surface, &surface_listener, this);
    xdg_toplevel_add_listener(m_xdg_toplevel, &toplevel_listener, this);

    createCursors();

    wl_surface_commit(m_surface);
    if (wl_display_roundtrip(m_display) == -1) {
        throw std::runtime_error("Wayland: Failed to wait for initial config.");
    }

    m_running = true;
}

static void convert_stb_rgba_to_wayland_argb8888(unsigned char* pixels, int width, int height) {
    const int total_pixels = width * height;

    for (int i = 0; i < total_pixels; i++) {
        unsigned char* p = &pixels[i * 4];

        unsigned char r = p[0];
        unsigned char g = p[1];
        unsigned char b = p[2];
        unsigned char a = p[3];

        // Premultiply alpha: (color * alpha) / 255
        // Write back in BGRA byte order (matches WL_SHM_FORMAT_ARGB8888 on little-endian)
        p[0] = (unsigned char)((b * a) / 255); // Blue
        p[1] = (unsigned char)((g * a) / 255); // Green
        p[2] = (unsigned char)((r * a) / 255); // Red
        p[3] = a;                              // Alpha
    }
}

void Wayland::createCursor(Cursor cursor, const std::string& filePath, int hotspotX, int hotspotY) {
    // Loading image
    auto [w, h, pixels] = file_utils::loadImage(filePath);
    convert_stb_rgba_to_wayland_argb8888(pixels, w, h);
    CursorImage image;
    // Creating surface
    image.surface = wl_compositor_create_surface(m_compositor);
    image.hotspotX = hotspotX;
    image.hotspotY = hotspotY;
    // Creating buffer and attaching to surface
    wl_buffer* buffer = createBuffer(m_shm, pixels, w, h);
    wl_surface_attach(image.surface, buffer, 0, 0);
    wl_surface_damage_buffer(image.surface, 0, 0, w, h);
    wl_surface_commit(image.surface);
    // Freeing resources
    wl_buffer_destroy(buffer);
    file_utils::freeImage(pixels);
    // Adding to cursor map
    mCursors.insert(std::pair(cursor, image));
}

void Wayland::createCursors() {
    createCursor(Cursor::ARROW,             "res/cursors/arrow32.png", 7, 7);
    createCursor(Cursor::ROTATE,            "res/cursors/rotate32.png", 15, 15);
    createCursor(Cursor::CROSSHAIR,         "res/cursors/crosshair32.png", 15, 15);
    createCursor(Cursor::SCALE_HORIZONTAL,  "res/cursors/scale_horizontal32.png", 15, 15);
    createCursor(Cursor::SCALE_VERTICAL,    "res/cursors/scale_vertical32.png", 15, 15);
    createCursor(Cursor::SHEAR_HORIZONTAL,  "res/cursors/shear_horizontal32.png", 15, 15);
    createCursor(Cursor::SHEAR_VERTICAL,    "res/cursors/shear_vertical32.png", 15, 15);
}

bool Wayland::shouldClose() const {
    // MUST BE IN THIS ORDER
    // Otherwise wl_display_dispatch() will block until the next event,
    // i.e., our window won't close until the user does something after
    // clicking the close button.
    return !m_running || (wl_display_dispatch(m_display) == -1);
}

void Wayland::drag() const {
    xdg_toplevel_move(m_xdg_toplevel, m_seat, m_last_button_serial);
}

void Wayland::showNativeWindowMenu() const {
    xdg_toplevel_show_window_menu(m_xdg_toplevel, m_seat, m_last_button_serial, m_pointer_x, m_pointer_y);
}

void Wayland::minimise() const {
    xdg_toplevel_set_minimized(m_xdg_toplevel);
}

void Wayland::toggleMaximise() const {
    if (m_maximised) {
        xdg_toplevel_unset_maximized(m_xdg_toplevel);
    } else {
        xdg_toplevel_set_maximized(m_xdg_toplevel);
    }
}

void Wayland::toggleFullscreen() const {
    if (m_fullscreen) {
        xdg_toplevel_unset_fullscreen(m_xdg_toplevel);
    } else {
        xdg_toplevel_set_fullscreen(m_xdg_toplevel, nullptr);
    }
}

void Wayland::pollEvents() const {
    pollfd pfd = {
        .fd = wl_display_get_fd(m_display),
        .events = POLLIN,
    };

    while (wl_display_prepare_read(m_display) != 0) {
        wl_display_dispatch_pending(m_display);
    }

    // Flushing the events we may have to read.
    wl_display_flush(m_display);

    if (poll(&pfd, 1, 0) > 0) {
        wl_display_read_events(m_display);
    } else {
        wl_display_cancel_read(m_display);
    }

    wl_display_dispatch_pending(m_display);
}

void Wayland::setCursor(Cursor cursor) {
    if (mCursors.contains(cursor)) {
        auto [surface, hotspotX, hotspotY] = mCursors[cursor];
        wl_pointer_set_cursor(m_pointer, m_last_enter_serial, surface, hotspotX, hotspotY);
    }
}
