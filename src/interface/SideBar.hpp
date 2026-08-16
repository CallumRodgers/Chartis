//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_SIDEBAR_HPP
#define OPENMAVIS_SIDEBAR_HPP

#include <imgui.h>
#include "dp.hpp"
#include "LogWindow.hpp"

namespace mavis::interface {
    class SideBar {
    public:
        static constexpr auto WINDOW_NAME = "SideBar";
        static constexpr auto WIDTH = 48_dp;

        void layout() {
            constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
            ImGui::Begin(WINDOW_NAME, nullptr, flags);

            if (ImGui::Button("Tree")) {

            }
            if (ImGui::Button("Time")) {

            }
            if (ImGui::Button("Calc")) {

            }
            if (ImGui::Button("Log")) {
                mLogWindowShowing = !mLogWindowShowing;
            }
            if (ImGui::Button("Settings")) {

            }

            ImGui::End();

            if (mLogWindowShowing) {
                mLogWindow.layout();
            }
        }
    private:
        LogWindow mLogWindow;
        bool mLogWindowShowing = false;
    };
};

#endif //OPENMAVIS_SIDEBAR_HPP
