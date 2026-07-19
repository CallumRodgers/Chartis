////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include "OpenMaVisApplication.hpp"

#include "interface/UserInterface.hpp"

#include <AMORA/Engine.hpp>

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/sinks/ConsoleSink.h>

using namespace mavis;

void OpenMaVisApplication::beginLogging() {
    quill::Backend::start();
    const auto sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("OpenMaVis_console_sink");
    logger = quill::Frontend::create_or_get_logger("OpenMaVis", sink);
    MAVIS_INFO("Started OpenMaVis logging");
}

void OpenMaVisApplication::endLogging() {
    MAVIS_INFO("Stopped OpenMaVis logging");
    logger->flush_log();
    quill::Backend::stop();
}

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv) : mWindow(createWindow()) {
    setupResizeCallback();
}

void OpenMaVisApplication::setupResizeCallback() {
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
}

void OpenMaVisApplication::run() {

    mainLoop();
    cleanUpAndTerminate();
}

void OpenMaVisApplication::mainLoop() {
    amora::Engine::startLogging();
    // By creating the Engine object here, it'll be automatically cleaned up
    // once it goes out of scope (i.e., the main loop exits).
    amora::Engine engine(mWindow);
    const amora::ImGuiEngineData data = engine.beginInterfaceBackend(mWindow);

    // Sets up the ImGui context on the application side, and sets up
    // interface styling, loads fonts, icons, etc.
    UserInterface::setup(data);

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
            engine.recreateSwapChain();
            mSurfaceInvalidated = false;
        }

        ////////////////////////////
        // RENDERING SCENE AND UI //
        ////////////////////////////

        engine.beginFrame();
        UserInterface::layout();
        engine.endFrame();

        // Updating frame variables and checking if we should idle the application.
        frameIndex++;
        if (frameIndex == maxContinuousFrames) {
            frameIndex = 0;
            waitEvents = true;
        }
    }

    engine.waitIdle();
    engine.endInterfaceBackend();
}

void OpenMaVisApplication::cleanUpAndTerminate() const {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void mavis::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->invalidateSurface();
}
