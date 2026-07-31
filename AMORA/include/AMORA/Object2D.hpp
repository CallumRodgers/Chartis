//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_RENDERIZABLEOBJECT_HPP
#define OPENMAVIS_RENDERIZABLEOBJECT_HPP

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

#endif //OPENMAVIS_RENDERIZABLEOBJECT_HPP
