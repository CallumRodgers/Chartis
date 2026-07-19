////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_USERINTERFACE_HPP
#define OPENMAVIS_USERINTERFACE_HPP

#include <AMORA/ImGuiEngineData.hpp>

namespace amora {
    class Engine;
}

namespace mavis {
    class UserInterface {
    public:
        static void setup(const amora::ImGuiEngineData& data);

        static void layout();

        static void end(const amora::Engine& engine);
    };
}

#endif //OPENMAVIS_USERINTERFACE_HPP
