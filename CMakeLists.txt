cmake_minimum_required(VERSION 4.1)
project(OpenMaVis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Finding required packages: GLFW and Vulkan.
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(slang REQUIRED)

# =========================================
# Vulkan setup.
# =========================================

find_package(Vulkan REQUIRED)

# =========================================
# Compiling third party libraries.
# =========================================

include(FetchContent)

# Quill
FetchContent_Declare(
        quill
        GIT_REPOSITORY https://github.com/odygrd/quill.git
        GIT_TAG        v12.0.0 # Use the specific version you want
)
FetchContent_MakeAvailable(quill)

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
        AMORA/include/AMORA/Engine.hpp
        AMORA/include/AMORA/Config.hpp
        AMORA/include/AMORA/AlgebraTypes.hpp
        AMORA/include/AMORA/Logger.hpp
        AMORA/include/AMORA/Scene.hpp
        AMORA/include/AMORA/Scene2D.hpp
        AMORA/include/AMORA/Scene3D.hpp

    PRIVATE
        AMORA/src/Engine.cpp

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
        AMORA/src/Pipeline.hpp
        AMORA/src/Pipeline.cpp
        AMORA/src/FenceGroup.hpp
        AMORA/src/SemaphoreGroup.hpp

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

        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_vulkan.cpp
)

target_include_directories(AMORA
        PUBLIC AMORA/include
        PRIVATE AMORA/src libs/imgui libs/imgui/backends
)

# This saves us from having to define these macros every time we include a GLFW/Vulkan header.
target_compile_definitions(AMORA PRIVATE
        GLFW_INCLUDE_NONE
        GLFW_INCLUDE_VULKAN
        VULKAN_HPP_NO_STRUCT_SETTERS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
)
# Precompilation speeds up the build process. It also takes it easier on the IDE parsing.
target_precompile_headers(AMORA PRIVATE <GLFW/glfw3.h> <vulkan/vulkan_raii.hpp>)

target_link_libraries(AMORA PRIVATE glfw Vulkan::Vulkan glm quill::quill)

# =========================================
# Listing source files.
# =========================================

set(SOURCES src/main.cpp
        # Sources
        src/OpenMaVisApplication.cpp

        # Headers
        include/OpenMaVisApplication.hpp
)

# =========================================
# Linking sources, modules, and libraries to executable.
# =========================================

add_executable(OpenMaVis ${SOURCES})
add_dependencies(OpenMaVis GraphicsShaders)

target_link_libraries(OpenMaVis PRIVATE AMORA glfw quill::quill)
target_include_directories(OpenMaVis PRIVATE include libs/imgui AMORA/include)
