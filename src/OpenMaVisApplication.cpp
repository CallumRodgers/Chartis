////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "OpenMaVisApplication.hpp"

using namespace MaVis;

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv) {
    mEngine = AMORA::Engine();
}

void OpenMaVisApplication::run() {
    init();
    mainLoop();
    cleanUpAndTerminate();
}

void OpenMaVisApplication::init() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW: Failed to initialize GLFW.");
    }
    if (!glfwVulkanSupported()) {
        throw std::runtime_error("GLFW: Vulkan support not available.");
    }
    createWindow();

    mEngine.initialise(mWindow);
}

void OpenMaVisApplication::mainLoop() {
    bool firstLoop = true;
    while (!glfwWindowShouldClose(mWindow)) {

        if (firstLoop) {
            mSurfaceInvalidated = true;
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

        ///////////////////////
        // RENDERING SURFACE //
        ///////////////////////

        if (mSurfaceInvalidated || mRenderMode == RENDER_MODE_CONTINUOUS) {
            mEngine.drawFrame();
            mSurfaceInvalidated = false;
        }
    }
}

void OpenMaVisApplication::cleanUpAndTerminate() {
    mEngine.terminate();
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    const auto app = static_cast<OpenMaVisApplication*>(glfwGetWindowUserPointer(window));
    app->invalidateSurface();
    app->mEngine.invalidateSwapChain();
}

void OpenMaVisApplication::createWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const int windowWidth = static_cast<int>(videoMode->width * 0.5);
    const int windowHeight = static_cast<int>(videoMode->height * 0.6);

    mWindow = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
    glfwSetCursor(mWindow, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
}
