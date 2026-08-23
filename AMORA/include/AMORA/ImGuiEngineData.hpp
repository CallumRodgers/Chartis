//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_IMGUIENGINEDATA_HPP
#define CHARTIS_IMGUIENGINEDATA_HPP

#include <imgui.h>

namespace amora {
    /// Structure to pass on ImGui instance data from the engine to the application.
    struct ImGuiEngineData {
        ImGuiContext* Context = nullptr;
        ImGuiMemAllocFunc AllocFunc = nullptr;
        ImGuiMemFreeFunc FreeFunc = nullptr;
        void* AllocUserData = nullptr;
    };
}

#endif //CHARTIS_IMGUIENGINEDATA_HPP
