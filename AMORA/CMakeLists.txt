cmake_minimum_required(VERSION 4.1)
project(AMORA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================
# THIRD-PARTY LIBRARIES
# =========================================

find_package(Vulkan REQUIRED)
find_package(slang REQUIRED)
find_package(glm REQUIRED)

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
        set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}")

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
        shaders/shader_vert.slang
        shaders/shader_frag.slang
        shaders/background_vert.slang
        shaders/background_frag.slang
        shaders/scene2D/coordinate_grid_2D_vert.slang
        shaders/scene2D/coordinate_grid_2D_cartesian.slang
        shaders/scene2D/coordinate_grid_2D_polar.slang
)
set(COMPUTE_SHADERS_SOURCES
)

compile_shaders(GraphicsShaders ${CMAKE_BINARY_DIR} ${GRAPHICS_SHADERS_SOURCES})
#compile_shaders(ComputeShaders ${CMAKE_CURRENT_BINARY_DIR} ${COMPUTE_SHADERS_SOURCES})

# =========================================
# AMORA Engine setup.
# =========================================

add_library(AMORA SHARED)
add_dependencies(AMORA GraphicsShaders)

target_sources(AMORA
        PUBLIC
        include/AMORA/AMORA.hpp
        include/AMORA/Engine.hpp
        include/AMORA/ImGuiEngineData.hpp
        include/AMORA/config.hpp
        include/AMORA/algebra_types.hpp
        include/AMORA/vector_utils.hpp
        include/AMORA/logger.hpp
        include/AMORA/FrameGroup.hpp
        include/AMORA/Scene.hpp
        include/AMORA/Scene2D.hpp
        #include/AMORA/Scene3D.hpp
        include/AMORA/Object2D.hpp

        PRIVATE
        src/Engine.cpp

        src/vulkan_utils.hpp

        src/VulkanContext.hpp
        src/VulkanContext.cpp
        src/RenderSurface.hpp
        src/RenderSurface.cpp
        src/CommandDispatcher.hpp
        src/CommandDispatcher.cpp
        src/SwapChain.hpp
        src/SwapChain.cpp
        src/StaticTexture.hpp
        src/StaticTexture.cpp
        src/TextureManager.hpp
        src/TextureManager.cpp
        src/Shader.hpp
        src/Shader.cpp
        src/DescriptorAllocator.hpp
        src/DescriptorAllocator.cpp
        src/DescriptorLayoutManager.hpp
        src/DescriptorLayoutManager.cpp
        src/DescriptorSetData.hpp
        src/DescriptorSetData.cpp
        src/UniformBuffer.hpp
        src/UniformBuffer.cpp
        src/Pipeline.hpp
        src/Pipeline.cpp
        src/FenceGroup.hpp
        src/SemaphoreGroup.hpp

        # Scene classes implementations
        src/scene/Scene.cpp
        src/scene/Scene2D.cpp
        src/scene/Background.hpp
        src/scene/Background.cpp
        src/scene/CoordinateGrid.hpp
        src/scene/CoordinateGrid.cpp
        src/scene/objects/Object2D.cpp
        #src/scene/Scene3D.cpp

        # Graphics classes implementations
        src/graphics/GraphicsPipeline.hpp
        src/graphics/GraphicsPipeline.cpp

        # Compute classes implementations
        src/compute/ComputePipeline.hpp
        src/compute/ComputePipeline.cpp
)

target_include_directories(AMORA
        PUBLIC include
        PRIVATE src
)

# This saves us from having to define these macros every time we include a Vulkan header.
target_compile_definitions(AMORA PRIVATE
        VULKAN_HPP_NO_STRUCT_SETTERS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
)
# Precompilation speeds up the build process. It also takes it easier on the IDE parsing.
target_precompile_headers(AMORA PRIVATE <vulkan/vulkan_raii.hpp>)

target_link_libraries(AMORA PRIVATE Chartis_Libs Chartis_Platform Vulkan::Vulkan glm)