////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
// Full license text available in the LICENSE file at the root of this repository.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "OpenMaVisApplication.h"

using namespace MaVis;

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv) {
    engine = AMORA::Engine();
}

void OpenMaVisApplication::run() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW: Failed to initialize GLFW.");
    }
    if (!glfwVulkanSupported()) {
        throw std::runtime_error("GLFW: Vulkan support not available.");
    }
    createWindow();

    engine.initialise(window);

    mainLoop();

    cleanUpResources();
    glfwTerminate();
}

void OpenMaVisApplication::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        engine.drawFrame();
    }

    engine.terminate();
}

void OpenMaVisApplication::cleanUpResources() {
    glfwDestroyWindow(window);
}

void OpenMaVisApplication::createWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const double proportion = 0.6;
    const int windowWidth = static_cast<int>(videoMode->width * proportion);
    const int windowHeight = static_cast<int>(videoMode->height * proportion);

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);
}
