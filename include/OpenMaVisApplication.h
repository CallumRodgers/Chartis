////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
// Full license text available in the LICENSE file at the root of this repository.
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_OPENMAVISAPPLICATION_H
#define OPENMAVIS_OPENMAVISAPPLICATION_H

#include "GLFW/glfw3.h"

import amora;

namespace MaVis {
    class OpenMaVisApplication {
    public:
        OpenMaVisApplication(int argc, char** argv);
        void run();

    private:
        /**
         * Main render loop.
         */
        void mainLoop();

        /**
         * Cleaning up resources before exit.
         */
        void cleanUpResources();

        AMORA::Engine engine;

        // ##########################################################
        // GLFW Functions and objects
        // ##########################################################

        void createWindow();

        GLFWwindow* window;
    };
}

#endif //OPENMAVIS_OPENMAVISAPPLICATION_H