////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_EVENTHANDLER2D_HPP
#define OPENMAVIS_EVENTHANDLER2D_HPP

#include <AMORA/Scene2D.hpp>

namespace mavis {
    /**
     *
     */
    class EventHandler2D {
    public:
        explicit EventHandler2D(const amora::Scene2D& scene) : mScene(scene) {}

        void processKeyEvent(int key, int scancode, int action, int mods);
        void processMouseMotionEvent();
        void processMouseButtonEvent();
        void processScrollEvent();

    private:
        const amora::Scene2D& mScene;
    };
}

#endif //OPENMAVIS_EVENTHANDLER2D_HPP
