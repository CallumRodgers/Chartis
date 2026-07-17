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
    /// Global logger for the engine.
    inline quill::Logger* logger = nullptr;

    constexpr int RENDER_MODE_DIRTY = 0;
    constexpr int RENDER_MODE_CONTINUOUS = 1;

    class OpenMaVisApplication {
    public:
        static void beginLogging();
        static void endLogging();

        OpenMaVisApplication(int argc, char** argv);

        GLFWwindow* mWindow;

        void run();

        void invalidateSurface() { mSurfaceInvalidated = true; };

        void invalidateRender() { mRenderInvalidated = true; }

        void setRenderMode(const int renderMode) {
            if (renderMode != RENDER_MODE_DIRTY && renderMode != RENDER_MODE_CONTINUOUS) {
                throw std::invalid_argument("Invalid render mode: " + std::to_string(renderMode));
            }
            mRenderMode = renderMode;
        };

    private:
        void mainLoop();

        void cleanUpAndTerminate() const;

        void setupResizeCallback();

        int mRenderMode = RENDER_MODE_DIRTY;
        bool mSurfaceInvalidated = false;
        bool mRenderInvalidated = false;

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
        const int windowWidth = static_cast<int>(videoMode->width * 0.5);
        const int windowHeight = static_cast<int>(videoMode->height * 0.6);

        GLFWwindow* win = glfwCreateWindow(windowWidth, windowHeight, "OpenMaVis", NULL, NULL);
        glfwSetCursor(win, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
        return win;
    }
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_HPP