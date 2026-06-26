cmake_minimum_required(VERSION 4.1)
project(OpenMaVis)

set(CMAKE_CXX_STANDARD 20)

# Finding required packages: GLFW and Vulkan.
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_PATH)

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
# Linking sources to executable.
# =========================================

add_executable(OpenMaVis ${SOURCES})
target_include_directories(OpenMaVis PRIVATE include)
target_link_libraries(OpenMaVis PRIVATE glfw Vulkan::Vulkan)