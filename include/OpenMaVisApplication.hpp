////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_HPP
#define OPENMAVIS_OPENMAVISAPPLICATION_HPP

#include "GLFW/glfw3.h"
#include <AMORA/Engine.hpp>

namespace MaVis {

    constexpr int RENDER_MODE_DIRTY = 0;
    constexpr int RENDER_MODE_CONTINUOUS = 1;

    class OpenMaVisApplication {
    public:
        OpenMaVisApplication(int argc, char** argv);

        GLFWwindow* mWindow;

        void run();

        void invalidateSurface() {
            mSurfaceInvalidated = true;
            mRenderInvalidated = true;
        };

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