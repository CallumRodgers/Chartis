cmake_minimum_required(VERSION 4.1)
project(Chartis LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================
# PLATFORM VARIABLES
# =========================================

set(Windows FALSE)
set(Apple FALSE)
set(Linux FALSE)

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
endif ()

# =========================================
# THIRD-PARTY LIBRARIES
# =========================================

include(FetchContent)

# ===================
# Quill
FetchContent_Declare(
        quill
        GIT_REPOSITORY https://github.com/odygrd/quill.git
        GIT_TAG        v12.0.0
)
FetchContent_MakeAvailable(quill)
# ===================

# ===================
# TOML++
FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)
# ===================

# =========================================
# Compiling XDG Shell protocol.
# =========================================

if (Linux)
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

# =========================================
# PROJECT LIBRARIES
# =========================================

add_subdirectory(AMORA)

# =========================================
# Listing source files.
# =========================================

set(SOURCES src/main.cpp
        # Sources
        src/ChartisApplication.hpp
        src/ChartisApplication.cpp
        src/file_utils.hpp
        src/key_converter.hpp

        src/control/EventHandler2D.hpp
        src/control/EventHandler2D.cpp
        src/control/EventHandler3D.hpp

        src/interface/dp.hpp
        src/interface/UserInterface.hpp
        src/interface/UserInterface.cpp
        src/interface/IconManager.hpp
        src/interface/IconManager.cpp
        src/interface/UIContext.hpp
        src/interface/MainBar.hpp
        src/interface/SideBar.hpp
        src/interface/theme/Theme.hpp
        src/interface/theme/Theme.cpp

        src/preferences/PreferenceManager.hpp
        src/preferences/PreferenceManager.cpp

        # Lib implementations
        libs/stb_image/stb_impl.cpp
)

# =========================================
# Listing platform code sources
# =========================================

set(SOURCES ${SOURCES}
        src/platform/NativeWindow.hpp
        src/platform/NativeWindow.cpp
)

if (Linux)
    set(SOURCES ${SOURCES}
            ${XDG_SHELL_IMPL}
            ${XDG_DECORATION_IMPL}
            src/platform/Wayland.hpp
            src/platform/Wayland.cpp
    )
endif ()

# =========================================
# Linking sources, modules, and libraries to executable.
# =========================================

add_executable(Chartis ${SOURCES})

target_link_libraries(Chartis PRIVATE AMORA quill::quill tomlplusplus::tomlplusplus)
target_include_directories(Chartis PRIVATE src libs/imgui libs/stb_image AMORA/include ${CMAKE_CURRENT_BINARY_DIR}/xdg)

if (Linux)
    target_link_libraries(Chartis PRIVATE ${WAYLAND_CLIENT_LIBRARY})
endif ()

add_custom_command(TARGET Chartis PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        $<TARGET_FILE_DIR:Chartis>/res
)