////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_SCENE_HPP
#define OPENMAVIS_SCENE_HPP

#include <AMORA/AlgebraTypes.hpp>
#include <string>

namespace AMORA {
    class Scene {
    public:
        Scene(const std::string& name) : mSceneName(name) {}

        void translate(m_vec3 a);
        void rotate(m_vec3 axis, m_float angle);

    protected:
        std::string mSceneName;

        m_mat4x4 mProjectionMatrix;
        m_mat4x4 mViewMatrix;
    };
}

#endif //OPENMAVIS_SCENE_HPP
