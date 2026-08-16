//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "OpenMaVisApplication.hpp"

#include "interface/UserInterface.hpp"
#include "backends/imgui_impl_glfw.h"

#include <AMORA/Engine.hpp>
#include <AMORA/Scene2D.hpp>
#include "file_utils.hpp"
#include "key_converter.hpp"

using namespace mavis;

void OpenMaVisApplication::createCursors(GLFWwindow* window) {
    float xscale, yscale;

    glfwGetWindowContentScale(window, &xscale, &yscale);

    GLFWimage cursorImage;
    int size;
    int scaleFactor;

    if (xscale >= 1.5f && yscale >= 1.5f) {
        size = 64;
        scaleFactor = 2;
    } else {
        size = 32;
        scaleFactor = 1;
    }

    cursorImage.width = size;
    cursorImage.height = size;

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/arrow" + std::to_string(size) + ".png"));
    CURSOR_ARROW = glfwCreateCursor(&cursorImage, 3 * scaleFactor, 3 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/rotate" + std::to_string(size) + ".png"));
    CURSOR_ROTATE = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/crosshair" + std::to_string(size) + ".png"));
    CURSOR_CROSSHAIR = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/scale_horizontal" + std::to_string(size) + ".png"));
    CURSOR_SCALE_HORIZONTAL = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/scale_vertical" + std::to_string(size) + ".png"));
    CURSOR_SCALE_VERTICAL = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/shear_horizontal" + std::to_string(size) + ".png"));
    CURSOR_SHEAR_HORIZONTAL = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);

    cursorImage.pixels = std::get<stbi_uc*>(file_utils::loadImage("res/cursors/shear_vertical" + std::to_string(size) + ".png"));
    CURSOR_SHEAR_VERTICAL = glfwCreateCursor(&cursorImage, 15 * scaleFactor, 15 * scaleFactor);
    file_utils::freeImage(cursorImage.pixels);
}

GLFWwindow* OpenMaVisApplication::createWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW: Failed to initialize GLFW.");
    }
    if (!glfwVulkanSupported()) {
        throw std::runtime_error("GLFW: Vulkan support not available.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // We have our own decoration.
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const int windowWidth = static_cast<int>(videoMode->width * 0.7);
    const int windowHeight = static_cast<int>(videoMode->height * 0.8);

    GLFWwindow* win = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);

    MAVIS_INFO("Created GLFW window");
    return win;
}

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv)
    : mWindow(createWindow()),
      mEventHandler2D(mWindow)
      //mEventHandler3D(mWindow)
{
    int w, h;
    glfwGetFramebufferSize(mWindow, &w, &h);
    mEventHandler2D.updateSurfaceSize(w, h);

    glfwSetWindowUserPointer(mWindow, this);

    //////////////////////////
    // SETUP GLFW CALLBACKS //
    //////////////////////////

    // Setting up resize callback.
    glfwSetFramebufferSizeCallback(mWindow, callbacks::onFramebufferResizeCallback);
    // Setting up EventHandler callbacks.
    glfwSetKeyCallback(mWindow, callbacks::onKeyEventCallback);
    glfwSetCursorPosCallback(mWindow, callbacks::onMouseMotionEventCallback);
    glfwSetMouseButtonCallback(mWindow, callbacks::onMouseButtonEventCallback);
    glfwSetScrollCallback(mWindow, callbacks::onScrollEventCallback);

    createCursors(mWindow);
}

void OpenMaVisApplication::toggleFullscreenMode() {
    const GLFWmonitor* currentMonitor = glfwGetWindowMonitor(mWindow);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (currentMonitor == nullptr) { // Window is in windowed mode.
        glfwGetWindowPos(mWindow, &mLastWindowX, &mLastWindowY);
        glfwGetWindowSize(mWindow, &mLastWindowWidth, &mLastWindowHeight);

        glfwSetWindowMonitor(mWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else { // Window is full-screen.
        glfwSetWindowMonitor(mWindow, nullptr, mLastWindowX, mLastWindowY, mLastWindowWidth, mLastWindowHeight, mode->refreshRate);
    }
}

void OpenMaVisApplication::run() {
    amora::Engine::startLogging();
    // By creating the Engine object here, it'll be automatically cleaned up
    // once it goes out of scope (i.e., the main loop exits).
    amora::Engine engine(mWindow);
    const amora::ImGuiEngineData data = engine.beginInterfaceBackend(mWindow);

    using interface::UserInterface;
    // Sets up the ImGui context on the application side, and sets up
    // interface styling, loads fonts, icons, etc.
    UserInterface ui(data);

    auto scene = amora::scene::Scene2D("Default");
    engine.loadScene(scene, true);

    mEventHandler2D.setActiveScene(&scene);

    // For our purposes we don't need to be rendering at V-Sync FPS non-stop if the
    // user is not interacting with us; we can idle the GPU usage to 0% after 5 frames
    // since the last input. The exception is if a scene is currently under animation,
    // in which case the engine will tell us, and the application will render continuously.
    constexpr int maxContinuousFrames = 5;
    int frameIndex = 0;
    bool waitEvents = false;
    // Obviously we want to render the first frame.
    mRenderInvalidated = true;

    while (!glfwWindowShouldClose(mWindow)) {
        //////////////////////
        // EVENT PROCESSING //
        //////////////////////

        if (waitEvents) {
            glfwWaitEvents();
            if (mRenderInvalidated) {
                waitEvents = false;
            }
        } else {
            glfwPollEvents();
        }

        if (mSurfaceInvalidated) {
            int w, h;
            glfwGetFramebufferSize(mWindow, &w, &h);
            engine.invalidateSurfaceSize(w, h);
            mEventHandler2D.updateSurfaceSize(w, h);
            mSurfaceInvalidated = false;
            mRenderInvalidated = true;
        }

        ////////////////////////////
        // RENDERING SCENE AND UI //
        ////////////////////////////

        if (mRenderInvalidated) {
            // Sets everything up for a new frame to be drawn.
            engine.beginFrame();
            ui.layout(this);
            // Submits the draw commands to the queue and presents the image.
            engine.endFrame();

            // Updating frame variables and checking if we should idle the application.
            frameIndex++;
            if (frameIndex == maxContinuousFrames) {
                frameIndex = 0;
                waitEvents = true;
                mRenderInvalidated = false;
            }
        }
    } // Main loop.

    engine.waitIdle();
    engine.endInterfaceBackend();
}

void OpenMaVisApplication::terminate() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

/////////////////////////////////////////////////////////////////////////////////////////
// GLFW CALLBACKS
/////////////////////////////////////////////////////////////////////////////////////////

void callbacks::onFramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->invalidateSurface();
}

void callbacks::onKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS && mods == 0) {
        app->toggleFullscreenMode();
    }

    auto& io = ImGui::GetIO();
    if (action == GLFW_PRESS) {
        io.AddKeyEvent(glfwKeyToImGuiKey(key), true);
    } else if (action == GLFW_RELEASE) {
        io.AddKeyEvent(glfwKeyToImGuiKey(key), false);
    }

    if (!io.WantCaptureKeyboard) {
        app->mEventHandler2D.processKeyEvent(key, scancode, action, mods);
    }

    app->invalidateRender();
}

void callbacks::onMouseMotionEventCallback(GLFWwindow* window, double xpos, double ypos) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));

    auto& io = ImGui::GetIO();
    io.AddMousePosEvent(static_cast<float>(xpos), static_cast<float>(ypos));

    if (!io.WantCaptureMouse) {
        app->mEventHandler2D.processMouseMotionEvent(xpos, ypos);
    }

    app->invalidateRender();
}

void callbacks::onMouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));

    auto& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);

    if (!io.WantCaptureMouse) {
        app->mEventHandler2D.processMouseButtonEvent(button, action, mods);
    }

    app->invalidateRender();
}

void callbacks::onScrollEventCallback(GLFWwindow* window, double xoffset, double yoffset) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));

    auto& io = ImGui::GetIO();
    io.AddMouseWheelEvent(xoffset, yoffset);

    if (!io.WantCaptureMouse) {
        app->mEventHandler2D.processScrollEvent(xoffset, yoffset);
    }

    app->invalidateRender();
}
