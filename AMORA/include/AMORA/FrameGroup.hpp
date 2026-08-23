//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_FRAMEGROUP_HPP
#define CHARTIS_FRAMEGROUP_HPP

#include <vector>
#include "config.hpp"

namespace amora {
    template <class T> class FrameGroup : public std::vector<T> {
    public:
        FrameGroup() : std::vector<T>(config::rendering::MAX_FRAMES_IN_FLIGHT) {}

    private:
    };
}

#endif //CHARTIS_FRAMEGROUP_HPP
