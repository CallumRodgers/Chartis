cmake_minimum_required(VERSION 4.1)
project(Chartis_Platform LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Chartis_Platform SHARED)

# =========================================
# PLATFORM VARIABLES
# =========================================

set(Windows FALSE)
set(Apple FALSE)
set(Linux FALSE)
set(IsWaylandSupported FALSE)
set(IsX11Supported FALSE)

if (WIN32)
    set(Windows TRUE)
elseif (APPLE)
    set(Apple TRUE)
elseif (UNIX AND NOT APPLE)
    set(Linux TRUE) # may also be FreeBSD, or any non-Apple UNIX system

    find_library(WAYLAND_CLIENT_LIBRARY
            NAMES wayland-client
            PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
    )

    if (WAYLAND_CLIENT_LIBRARY)
        set(IsWaylandSupported TRUE)
    endif ()

    set(IsX11Supported TRUE) # Temporary
endif ()

if (Linux AND IsWaylandSupported)
    # =========================================
    # Compiling XDG protocols.
    # =========================================

    set(XDG_SHELL_XML /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml)
    set(XDG_DECORATION_XML /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml)

    set(XDG_DIR ${CMAKE_CURRENT_BINARY_DIR}/xdg)
    file(MAKE_DIRECTORY ${XDG_DIR})

    set(XDG_SHELL_HEADER ${XDG_DIR}/xdg-shell-client-protocol.h)
    set(XDG_SHELL_IMPL ${XDG_DIR}/xdg-shell-protocol.c)
    message("Compiling XDG Shell protocol")
    execute_process(COMMAND wayland-scanner client-header ${XDG_SHELL_XML} ${XDG_SHELL_HEADER})
    execute_process(COMMAND wayland-scanner private-code ${XDG_SHELL_XML} ${XDG_SHELL_IMPL})

    set(XDG_DECORATION_HEADER ${XDG_DIR}/xdg-decoration-client-protocol.h)
    set(XDG_DECORATION_IMPL ${XDG_DIR}/xdg-decoration-protocol.c)
    message("Compiling xdg-decoration protocol")
    execute_process(COMMAND wayland-scanner client-header ${XDG_DECORATION_XML} ${XDG_DECORATION_HEADER})
    execute_process(COMMAND wayland-scanner private-code ${XDG_DECORATION_XML} ${XDG_DECORATION_IMPL})
endif ()

set(SOURCES_PUBLIC
        include/platypus/NativeWindow.hpp
        include/platypus/UserInput.hpp
)
set(SOURCES_PRIVATE
        src/NativeWindow.cpp

        src/platform_file_utils.hpp
        src/platform_logger.hpp
)

if (Linux)
    if (IsWaylandSupported)
        set(SOURCES_PRIVATE ${SOURCES_PRIVATE}
                src/Wayland.hpp
                src/Wayland.cpp
                src/wayland_converter.hpp

                # Wayland protocols
                ${XDG_SHELL_IMPL}
                ${XDG_DECORATION_IMPL}
        )
        target_link_libraries(Chartis_Platform PRIVATE ${WAYLAND_CLIENT_LIBRARY})
        target_include_directories(Chartis_Platform PRIVATE ${XDG_DIR})
    endif ()
    if (IsX11Supported)
        set(SOURCES_PUBLIC ${SOURCES_PUBLIC}
                src/X11.hpp
                src/X11.cpp
        )

    endif ()
endif ()

target_sources(Chartis_Platform PUBLIC ${SOURCES_PUBLIC} PRIVATE ${SOURCES_PRIVATE})
target_link_libraries(Chartis_Platform PRIVATE Chartis_Libs)
target_include_directories(Chartis_Platform PUBLIC include)
