cmake_minimum_required(VERSION 4.1)
project(Chartis_Libs LANGUAGES C CXX)

add_library(Chartis_Libs SHARED)
target_sources(Chartis_Libs
        PRIVATE
        # STB libraries
        stb/stb_impl.cpp

        # =========================================
        # Dear ImGui Sources
        # =========================================

        imgui/imgui.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp

        imgui/backends/imgui_impl_vulkan.cpp
)

# =========================================
# EXTERNAL LIBRARIES
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

target_link_libraries(Chartis_Libs PUBLIC quill::quill)
target_include_directories(Chartis_Libs
        PUBLIC
        stb imgui
)
