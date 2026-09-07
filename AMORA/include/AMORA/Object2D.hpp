////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_RENDERIZABLEOBJECT_HPP
#define CHARTIS_RENDERIZABLEOBJECT_HPP

#include "algebra_types.hpp"

namespace amora::scene {

    struct Vertex2D {
        m_vec2 pos;
    };

    class Object2D {
    public:
        virtual ~Object2D();

        //virtual void setupVertices() = 0;

    protected:

        m_mat4x4 mTransformationMatrix;


        std::vector<Vertex2D> vertices;


    };

}

#endif //CHARTIS_RENDERIZABLEOBJECT_HPP
