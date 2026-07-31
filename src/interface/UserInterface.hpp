//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_USERINTERFACE_HPP
#define OPENMAVIS_USERINTERFACE_HPP

#include <AMORA/ImGuiEngineData.hpp>

namespace amora {
    class Engine;
}

namespace mavis::interface {
    class UserInterface {
    public:
        static void setup(const amora::ImGuiEngineData& data);

        static void layout();

        static void end(const amora::Engine& engine);
    };
}

#endif //OPENMAVIS_USERINTERFACE_HPP
