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

set(GRAPHICS_SHADERS_SOURCES AMORA/shaders/shader.slang)
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
        FILE_SET CXX_MODULES
        FILES
            AMORA/AMORA.cppm
            AMORA/graphics/graphics.cppm
            AMORA/compute/compute.cppm
    PRIVATE
        # AMORA classes implementations
        AMORA/Engine.cpp
        AMORA/Shader.cpp
        AMORA/Pipeline.cpp

        # Graphics classes implementations
        AMORA/graphics/GraphicsPipeline.cpp

        # Compute classes implementations
        AMORA/compute/ComputePipeline.cpp
)

target_compile_features(AMORA PUBLIC cxx_std_20)
target_link_libraries(AMORA PRIVATE glfw Vulkan::Vulkan glm)

# =========================================
# Listing source files.
# =========================================

set(SOURCES src/main.cpp
        # Sources
        src/OpenMaVisApplication.cpp

        # Headers
        include/OpenMaVisApplication.h
)

# =========================================
# Linking sources, modules, and libraries to executable.
# =========================================

add_executable(OpenMaVis ${SOURCES})
add_dependencies(OpenMaVis GraphicsShaders)

target_link_libraries(OpenMaVis PRIVATE AMORA glfw)
target_include_directories(OpenMaVis PRIVATE include)
