////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_USERINTERFACE_HPP
#define OPENMAVIS_USERINTERFACE_HPP

#include <GLFW/glfw3.h>

namespace amora {
    class Engine;
}

namespace mavis {
    class UserInterface {
    public:
        static void begin(GLFWwindow* window, const amora::Engine& engine);
        static void end(const amora::Engine& engine);
    };
}

#endif //OPENMAVIS_USERINTERFACE_HPP
