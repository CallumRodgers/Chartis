#include "OpenMaVisApplication.h"

#include <stdexcept>

OpenMaVisApplication::OpenMaVisApplication(int argc, char** argv) {}

void OpenMaVisApplication::run() {
    initialise();
    mainLoop();
    cleanup();
}

void OpenMaVisApplication::initialise() {
    initWindow();
    initVulkan();
}

void OpenMaVisApplication::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void OpenMaVisApplication::cleanup() {
    glfwDestroyWindow(window);

    glfwTerminate();
}

void OpenMaVisApplication::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    if (!glfwVulkanSupported()) {
        throw std::runtime_error("Vulkan support not available.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    const double proportion = 0.6;
    const int windowWidth = static_cast<int>(videoMode->width * proportion);
    const int windowHeight = static_cast<int>(videoMode->height * proportion);

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);
}

void OpenMaVisApplication::initVulkan() {

}
