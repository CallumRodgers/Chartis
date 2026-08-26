cmake_minimum_required(VERSION 4.1)
project(Chartis LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================
# THIRD-PARTY LIBRARIES
# =========================================

include(FetchContent)

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
# PROJECT LIBRARIES
# =========================================

# AMORA Engine
add_subdirectory(AMORA)
# Chartis platform-specific code
add_subdirectory(platypus)
# Local Third-party libraries
add_subdirectory(libs)

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
)

# =========================================
# Linking sources, modules, and libraries to executable.
# =========================================

add_executable(Chartis ${SOURCES})

target_link_libraries(Chartis PRIVATE AMORA Chartis_Libs Chartis_Platform tomlplusplus::tomlplusplus)
target_include_directories(Chartis PRIVATE src)

add_custom_command(TARGET Chartis PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        $<TARGET_FILE_DIR:Chartis>/res
)