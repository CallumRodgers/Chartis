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
    MAVIS_INFO("Successfully initialized OpenMaVis logger.");
}

void OpenMaVisApplication::endLogging() {
    MAVIS_INFO("Ending OpenMaVis logging.");
    logger->flush_log();
    quill::Backend::stop();
}

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv)
    : mWindow(createWindow())
{
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
    amora::ImGuiEngineData data = engine.beginUserInterface(mWindow);

    setRenderMode(RENDER_MODE_CONTINUOUS);

    //ImGui::SetAllocatorFunctions(data.AllocFunc, data.FreeFunc, data.AllocUserData);
    //ImGui::SetCurrentContext(data.Context);

    bool firstLoop = true;
    while (!glfwWindowShouldClose(mWindow)) {

        if (firstLoop) {
            mRenderInvalidated = true;
        }

        //////////////////////
        // EVENT PROCESSING //
        //////////////////////

        if (firstLoop) {
            glfwPollEvents();
            firstLoop = false;
        } else if (mRenderMode == RENDER_MODE_DIRTY) {
            glfwWaitEvents();
        } else { // RENDER_MODE_CONTINUOUS
            glfwPollEvents();
        }

        if (mSurfaceInvalidated) {
            engine.invalidateSwapChain();
            mSurfaceInvalidated = false;
            mRenderInvalidated = true;
        }

        ///////////////////////
        // RENDERING SURFACE //
        ///////////////////////

        if (mRenderInvalidated || mRenderMode == RENDER_MODE_CONTINUOUS) {
            engine.renderActive();
            mRenderInvalidated = false;
        }
    }

    engine.endUserInterface();
}

void OpenMaVisApplication::cleanUpAndTerminate() const {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void mavis::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->invalidateSurface();
}
