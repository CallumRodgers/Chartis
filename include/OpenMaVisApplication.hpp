////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_HPP
#define OPENMAVIS_OPENMAVISAPPLICATION_HPP

#include <GLFW/glfw3.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>

#ifdef NDEBUG
#define MAVIS_DEBUG(...)
#else
#define MAVIS_DEBUG(...) LOG_DEBUG(mavis::logger, __VA_ARGS__)
#endif

#define MAVIS_INFO(...) LOG_INFO(mavis::logger, __VA_ARGS__)
#define MAVIS_WARN(...) LOG_WARNING(mavis::logger, __VA_ARGS__)
#define MAVIS_ERROR(...) LOG_ERROR(mavis::logger, __VA_ARGS__)

namespace mavis {
    constexpr auto APPLICATION_VERSION = "0.1.0";

    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;

    class OpenMaVisApplication {
    public:
        static void beginLogging();
        static void endLogging();

        OpenMaVisApplication(int argc, char** argv);

        GLFWwindow* mWindow;

        void run();

        void invalidateSurface() { mSurfaceInvalidated = true; };

    private:
        void mainLoop();

        void cleanUpAndTerminate() const;

        void setupResizeCallback();

        bool mSurfaceInvalidated = false;

        /// Resizing operation doesn't need more than 30 FPS.
        int mMinResizeDelay = static_cast<int>(1.0f / 30.0f);

    };

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    static GLFWwindow* createWindow() {
        if (!glfwInit()) {
            throw std::runtime_error("GLFW: Failed to initialize GLFW.");
        }
        if (!glfwVulkanSupported()) {
            throw std::runtime_error("GLFW: Vulkan support not available.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        const int windowWidth = static_cast<int>(videoMode->width * 0.7);
        const int windowHeight = static_cast<int>(videoMode->height * 0.8);

        GLFWwindow* win = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);
        glfwSetCursor(win, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
        return win;
    }
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_HPP