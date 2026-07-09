////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_HPP
#define OPENMAVIS_OPENMAVISAPPLICATION_HPP

#include "GLFW/glfw3.h"

import amora;

namespace MaVis {

    constexpr int RENDER_MODE_DIRTY = 0;
    constexpr int RENDER_MODE_CONTINUOUS = 1;

    class OpenMaVisApplication {
    public:
        OpenMaVisApplication(int argc, char** argv);

        AMORA::Engine mEngine;

        void run();
        void invalidateSurface() {
            mSurfaceInvalidated = true;
        };

        void setRenderMode(const int renderMode) {
            if (renderMode != RENDER_MODE_DIRTY && renderMode != RENDER_MODE_CONTINUOUS) {
                throw std::invalid_argument("Invalid render mode: " + std::to_string(renderMode));
            }
            mRenderMode = renderMode;
        };

    private:
        /**
         * Initializes GLFW, the window, the engine, and everything else.
         */
        void init();

        /**
         * Main render loop.
         */
        void mainLoop();

        /**
         * Cleaning up resources before exit.
         */
        void cleanUpAndTerminate();

        // ##########################################################
        // GLFW Functions and objects
        // ##########################################################

        void createWindow();

        GLFWwindow* mWindow = nullptr;

        int mRenderMode = RENDER_MODE_DIRTY;
        bool mSurfaceInvalidated = false;

    };
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_HPP