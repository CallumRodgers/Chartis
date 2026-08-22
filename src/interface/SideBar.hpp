//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_SIDEBAR_HPP
#define OPENMAVIS_SIDEBAR_HPP

#include <imgui.h>
#include "dp.hpp"
#include "IconManager.hpp"
#include "LogWindow.hpp"

namespace mavis::interface {
    class SideBar {
    public:
        static constexpr auto WINDOW_NAME = "SideBar";
        static constexpr auto WIDTH = 40_dp;

        void layout(const UIContext& context) {
            constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8_dp, 8_dp));

            ImGui::Begin(WINDOW_NAME, nullptr, flags);

            const auto buttonSize = ImVec2(WIDTH - 16_dp, WIDTH - 16_dp);

            auto& iconManager = context.iconManager;

            if (ImGui::ImageButton("Tree", iconManager.getIconImage(TREE), buttonSize)) {

            }
            if (ImGui::ImageButton("Time", iconManager.getIconImage(PLAY_PAUSE), buttonSize)) {

            }
            if (ImGui::ImageButton("Calculator", iconManager.getIconImage(CALCULATOR), buttonSize)) {

            }
            if (ImGui::ImageButton("Log", iconManager.getIconImage(LOG), buttonSize)) {
                mLogWindowShowing = !mLogWindowShowing;
            }
            if (ImGui::ImageButton("Settings", iconManager.getIconImage(SETTINGS), buttonSize)) {

            }

            ImGui::End();
            ImGui::PopStyleVar(3);

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
