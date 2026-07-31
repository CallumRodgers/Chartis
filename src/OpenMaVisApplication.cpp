//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "OpenMaVisApplication.hpp"

#include "interface/UserInterface.hpp"

#include <AMORA/Engine.hpp>
#include <AMORA/Scene2D.hpp>

using namespace mavis;

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
}

void OpenMaVisApplication::toggleFullscreenMode() {
    GLFWmonitor* currentMonitor = glfwGetWindowMonitor(mWindow);

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

    mainLoop();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void OpenMaVisApplication::mainLoop() {
    amora::Engine::startLogging();
    // By creating the Engine object here, it'll be automatically cleaned up
    // once it goes out of scope (i.e., the main loop exits).
    amora::Engine engine(mWindow);
    const amora::ImGuiEngineData data = engine.beginInterfaceBackend(mWindow);

    using interface::UserInterface;
    // Sets up the ImGui context on the application side, and sets up
    // interface styling, loads fonts, icons, etc.
    UserInterface::setup(data);

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

    while (!glfwWindowShouldClose(mWindow)) {
        //////////////////////
        // EVENT PROCESSING //
        //////////////////////

        if (waitEvents) {
            glfwWaitEvents();
            waitEvents = false;
        } else {
            glfwPollEvents();
        }

        if (mSurfaceInvalidated) {
            int w, h;
            glfwGetFramebufferSize(mWindow, &w, &h);
            engine.invalidateSurfaceSize(w, h);
            mEventHandler2D.updateSurfaceSize(w, h);
            mSurfaceInvalidated = false;
        }

        ////////////////////////////
        // RENDERING SCENE AND UI //
        ////////////////////////////

        // Sets everything up for a new frame to be drawn.
        engine.beginFrame();
        // Not using the UI for now in this branch.
        //UserInterface::layout();
        // Submits the draw commands to the queue and presents the image.
        engine.endFrame();

        // Updating frame variables and checking if we should idle the application.
        frameIndex++;
        if (frameIndex == maxContinuousFrames) {
            frameIndex = 0;
            waitEvents = true;
        }
    } // Main loop.

    engine.waitIdle();
    engine.endInterfaceBackend();
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
    app->mEventHandler2D.processKeyEvent(key, scancode, action, mods);
}

void callbacks::onMouseMotionEventCallback(GLFWwindow* window, double xpos, double ypos) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->mEventHandler2D.processMouseMotionEvent(xpos, ypos);
}

void callbacks::onMouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->mEventHandler2D.processMouseButtonEvent(button, action, mods);
}

void callbacks::onScrollEventCallback(GLFWwindow* window, double xoffset, double yoffset) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->mEventHandler2D.processScrollEvent(xoffset, yoffset);
}
