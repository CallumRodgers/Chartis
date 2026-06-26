#ifndef OPENGRAPHER_OPENGRAPHERAPPLICATION_H
#define OPENGRAPHER_OPENGRAPHERAPPLICATION_H

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

class OpenMaVisApplication {
    public:
        OpenMaVisApplication(int argc, char** argv);
        void run();

    private:
        /**
         * Start GLFW and Vulkan. Prepare drawing environment.
         */
        void initialise();

        /**
         * Main render loop.
         */
        void mainLoop();

        /**
         * Cleaning up resources before exit.
         */
        void cleanup();

        void initWindow();
        void initVulkan();

        GLFWwindow* window;
};

#endif //OPENGRAPHER_OPENGRAPHERAPPLICATION_H