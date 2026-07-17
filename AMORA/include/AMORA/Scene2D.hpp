////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_SCENE2D_HPP
#define OPENMAVIS_SCENE2D_HPP

#include <AMORA/Scene.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace amora {
    class Scene2D final : public Scene {
        Scene2D(const std::string& name) : Scene(name) {
            mProjectionMatrix = glm::ortho();
        }
    };
}

#endif //OPENMAVIS_SCENE2D_HPP
