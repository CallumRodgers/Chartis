cmake_minimum_required(VERSION 4.1)
project(OpenMaVis)

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

find_package(Vulkan REQUIRED)
find_package(slang REQUIRED)
find_package(glm REQUIRED)

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
# Creating function for shader compilation.
# =========================================

function(compile_shaders TARGET_NAME OUTPUT_DIR)
    # Shader files are specified manually.
    set(SHADER_FILES ${ARGN})

    if(NOT SHADER_FILES)
        message(WARNING "compile_shaders called with no shader files for target: ${TARGET_NAME}")
        return()
    endif()

    # Declaring variable to hold the output files that the target will depend on.
    set(SPV_OUTPUT_FILES "")

    foreach(SHADER_FILE ${SHADER_FILES})
        set(SRC_PATH "${CMAKE_SOURCE_DIR}/${SHADER_FILE}")

        file(RELATIVE_PATH REL_TO_ROOT "${CMAKE_SOURCE_DIR}/AMORA/shaders" ${SRC_PATH})

        # Get relative subdir.
        get_filename_component(RELATIVE_DIR ${REL_TO_ROOT} DIRECTORY)

        # File name (W)ithout (E)xtension.
        get_filename_component(FILE_NAME_WE ${REL_TO_ROOT} NAME_WE)

        if(RELATIVE_DIR)
            set(SPV_PATH "${OUTPUT_DIR}/${RELATIVE_DIR}/${FILE_NAME_WE}.spv")
        else()
            set(SPV_PATH "${OUTPUT_DIR}/${FILE_NAME_WE}.spv")
        endif()

        # We need to create this subdir within the output destination if it doesn't exist.
        get_filename_component(SPV_SUBDIR ${SPV_PATH} DIRECTORY)

        # Actually compiling.
        add_custom_command(
                OUTPUT ${SPV_PATH}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SPV_SUBDIR}
                COMMAND ${SLANGC_EXECUTABLE} ${SRC_PATH} -target spirv -profile spirv_1_4 -emit-spirv-directly -o ${SPV_PATH}
                DEPENDS ${SRC_PATH}
                COMMENT "Compiling Slang shader: ${SRC_PATH} -> ${SPV_PATH}"
                VERBATIM
        )

        list(APPEND SPV_OUTPUT_FILES ${SPV_PATH})
    endforeach()

    add_custom_target(${TARGET_NAME} DEPENDS ${SPV_OUTPUT_FILES})
endfunction()

# =========================================
# Compiling shaders.
# =========================================

set(GRAPHICS_SHADERS_SOURCES
        AMORA/shaders/shader_vert.slang
        AMORA/shaders/shader_frag.slang
        AMORA/shaders/background_vert.slang
        AMORA/shaders/background_frag.slang
        AMORA/shaders/scene2D/coordinate_grid_2D_vert.slang
        AMORA/shaders/scene2D/coordinate_grid_2D_cartesian.slang
        AMORA/shaders/scene2D/coordinate_grid_2D_polar.slang
)
set(COMPUTE_SHADERS_SOURCES
)

compile_shaders(GraphicsShaders ${CMAKE_BINARY_DIR} ${GRAPHICS_SHADERS_SOURCES})
#compile_shaders(ComputeShaders ${CMAKE_CURRENT_BINARY_DIR} ${COMPUTE_SHADERS_SOURCES})

# =========================================
# AMORA Engine setup.
# =========================================

add_library(AMORA SHARED)
target_sources(AMORA
    PUBLIC
        AMORA/include/AMORA/AMORA.hpp
        AMORA/include/AMORA/Engine.hpp
        AMORA/include/AMORA/Platform.hpp
        AMORA/include/AMORA/ImGuiEngineData.hpp
        AMORA/include/AMORA/config.hpp
        AMORA/include/AMORA/algebra_types.hpp
        AMORA/include/AMORA/vector_utils.hpp
        AMORA/include/AMORA/logger.hpp
        AMORA/include/AMORA/FrameGroup.hpp
        AMORA/include/AMORA/Scene.hpp
        AMORA/include/AMORA/Scene2D.hpp
        #AMORA/include/AMORA/Scene3D.hpp
        AMORA/include/AMORA/Object2D.hpp

    PRIVATE
        AMORA/src/Engine.cpp

        AMORA/src/vulkan_utils.hpp

        AMORA/src/VulkanContext.hpp
        AMORA/src/VulkanContext.cpp
        AMORA/src/RenderSurface.hpp
        AMORA/src/RenderSurface.cpp
        AMORA/src/CommandDispatcher.hpp
        AMORA/src/CommandDispatcher.cpp
        AMORA/src/SwapChain.hpp
        AMORA/src/SwapChain.cpp
        AMORA/src/Shader.hpp
        AMORA/src/Shader.cpp
        AMORA/src/DescriptorAllocator.hpp
        AMORA/src/DescriptorAllocator.cpp
        AMORA/src/DescriptorLayoutManager.hpp
        AMORA/src/DescriptorLayoutManager.cpp
        AMORA/src/DescriptorSetData.hpp
        AMORA/src/DescriptorSetData.cpp
        AMORA/src/UniformBuffer.hpp
        AMORA/src/UniformBuffer.cpp
        AMORA/src/Pipeline.hpp
        AMORA/src/Pipeline.cpp
        AMORA/src/FenceGroup.hpp
        AMORA/src/SemaphoreGroup.hpp

        # Scene classes implementations
        AMORA/src/scene/Scene.cpp
        AMORA/src/scene/Scene2D.cpp
        AMORA/src/scene/Background.hpp
        AMORA/src/scene/Background.cpp
        AMORA/src/scene/CoordinateGrid.hpp
        AMORA/src/scene/CoordinateGrid.cpp
        AMORA/src/scene/objects/Object2D.cpp
        #AMORA/src/scene/Scene3D.cpp

        # Graphics classes implementations
        AMORA/src/graphics/GraphicsPipeline.hpp
        AMORA/src/graphics/GraphicsPipeline.cpp

        # Compute classes implementations
        AMORA/src/compute/ComputePipeline.hpp
        AMORA/src/compute/ComputePipeline.cpp

        # =========================================
        # Dear ImGui Sources
        # =========================================

        libs/imgui/imgui.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_demo.cpp

        libs/imgui/backends/imgui_impl_vulkan.cpp
)

target_include_directories(AMORA
        PUBLIC AMORA/include
        PRIVATE AMORA/src libs/imgui
)

# This saves us from having to define these macros every time we include a GLFW/Vulkan header.
target_compile_definitions(AMORA PRIVATE
        VULKAN_HPP_NO_STRUCT_SETTERS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
)
# Precompilation speeds up the build process. It also takes it easier on the IDE parsing.
target_precompile_headers(AMORA PRIVATE <vulkan/vulkan_raii.hpp>)

target_link_libraries(AMORA PRIVATE Vulkan::Vulkan glm quill::quill)

# =========================================
# Listing source files.
# =========================================

set(SOURCES src/main.cpp
        # Sources
        src/OpenMaVisApplication.hpp
        src/OpenMaVisApplication.cpp
        src/file_utils.hpp
        src/key_converter.hpp

        src/control/EventHandler2D.hpp
        src/control/EventHandler2D.cpp
        src/control/EventHandler3D.hpp

        src/interface/dp.hpp
        src/interface/UserInterface.hpp
        src/interface/UserInterface.cpp
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

add_executable(OpenMaVis ${SOURCES})
add_dependencies(OpenMaVis GraphicsShaders)

target_link_libraries(OpenMaVis PRIVATE AMORA quill::quill tomlplusplus::tomlplusplus)
target_include_directories(OpenMaVis PRIVATE src libs/imgui libs/stb_image AMORA/include ${CMAKE_CURRENT_BINARY_DIR}/xdg)

if (Linux)
    target_link_libraries(OpenMaVis PRIVATE ${WAYLAND_CLIENT_LIBRARY})
endif ()

add_custom_command(TARGET OpenMaVis PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        $<TARGET_FILE_DIR:OpenMaVis>/res
)