////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "OpenMaVisApplication.hpp"

using namespace MaVis;

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
    AMORA::Engine::startLogging();
    // By creating the Engine object here, it'll be automatically cleaned up
    // once it goes out of scope (i.e., the main loop exits).
    AMORA::Engine engine(mWindow);

    bool firstLoop = true;
    while (!glfwWindowShouldClose(mWindow)) {

        if (firstLoop) {
            mRenderInvalidated = true;
        }

        //////////////////////
        // EVENT PROCESSING //
        //////////////////////

        if (mSurfaceInvalidated) {
            engine.invalidateSwapChain();
            mSurfaceInvalidated = false;
        }

        if (firstLoop) {
            glfwPollEvents();
            firstLoop = false;
        } else if (mRenderMode == RENDER_MODE_DIRTY) {
            glfwWaitEvents();
        } else { // RENDER_MODE_CONTINUOUS
            glfwPollEvents();
        }

        ///////////////////////
        // RENDERING SURFACE //
        ///////////////////////

        if (mRenderInvalidated || mRenderMode == RENDER_MODE_CONTINUOUS) {
            engine.renderActive();
            mRenderInvalidated = false;
        }
    }
}

void OpenMaVisApplication::cleanUpAndTerminate() const {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void MaVis::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->invalidateSurface();
}
