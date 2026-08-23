//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_MAINMENUBAR_HPP
#define CHARTIS_MAINMENUBAR_HPP

#include <imgui.h>
#include "dp.hpp"
#include "ChartisApplication.hpp"
#include "UIContext.hpp"

namespace chartis::interface {
    class MainBar {
    public:
        void layout(const UIContext& context) {

            auto& iconManager = context.iconManager;

            const float desiredHeight = 40_dp;
            const float fontSize = ImGui::GetFontSize();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, (desiredHeight - fontSize) / 2.f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
            ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.f);

            if (ImGui::BeginMainMenuBar()) {

                const auto desiredSize = ImGui::GetFrameHeight();
                const auto framePadding = ImGui::GetStyle().FramePadding;

                const auto windowButtonsSize = ImVec2(desiredSize - 2 * framePadding.x, desiredSize - 2 * framePadding.y);
                const float windowButtonsPos = ImGui::GetWindowViewport()->Size.x - 3 * desiredSize;
                const auto windowButtonSizeRatio = windowButtonsSize.y / windowButtonsSize.x;

                const auto uv0 = ImVec2(0.f, 0.5f - windowButtonSizeRatio / 2);
                const auto uv1 = ImVec2(1.f, 0.5f + windowButtonSizeRatio / 2);

                ImGui::SetCursorPosX(0.f);
                if (ImGui::ImageButton("Menu", iconManager.getIconImage(MENU), windowButtonsSize, uv0, uv1)) {
                    mIsMainMenuShowing = !mIsMainMenuShowing;
                    context.app->invalidateRender();
                }

                if (mIsMainMenuShowing) {
                    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.0f);

                    if (ImGui::BeginMenu("Chartis")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Scene")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("View")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Export")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Help")) {

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("About")) {

                        ImGui::EndMenu();
                    }
                    ImGui::PopStyleVar();
                } else {
                    if (ImGui::BeginTabBar("SceneTabs")) {

                        if (ImGui::BeginTabItem("Default")) {

                            ImGui::EndTabItem();
                        }

                        if (ImGui::ImageButton("AddTab", iconManager.getIconImage(PLUS), windowButtonsSize, uv0, uv1)) {
                            CHARTIS_INFO("NEW TAB!");
                        }

                        ImGui::EndTabBar();
                    }
                }

                ////////////////////
                // WINDOW BUTTONS //
                ////////////////////

                ImGui::SameLine();

                ImGui::SetCursorPosX(windowButtonsPos);
                ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0.f);

                const bool isMaximised = context.app->mNativeWindow.isMaximised();
                const bool isFullscreen = context.app->mNativeWindow.isFullscreen();

                if (!isFullscreen) {
                    if (ImGui::ImageButton("Minimise", iconManager.getIconImage(MINIMISE), windowButtonsSize, uv0, uv1)) {
                        context.app->mNativeWindow.minimise();
                    }
                    ImGui::SameLine();
                } else {
                    // Need to take the missing button into account for the layout.
                    ImGui::SetCursorPosX(windowButtonsPos + desiredSize);
                }

                const ImTextureID maximiseIcon = isMaximised || isFullscreen ? iconManager.getIconImage(COLLAPSE) : iconManager.getIconImage(MAXIMISE);
                if (ImGui::ImageButton("Maximise", maximiseIcon, windowButtonsSize, uv0, uv1)) {
                    if (isFullscreen) {
                        context.app->mNativeWindow.toggleFullscreen();
                    } else {
                        context.app->mNativeWindow.toggleMaximise();
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton("Close", iconManager.getIconImage(CLOSE), windowButtonsSize, uv0, uv1)) {
                    context.app->mNativeWindow.close();
                }


                ImGui::PopStyleVar();

                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup)) {
                    if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
                        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            context.app->mNativeWindow.drag();
                        } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                            context.app->mNativeWindow.showNativeWindowMenu();
                        }
                    }
                }

                ImGui::EndMainMenuBar();
            }
            ImGui::PopStyleVar(3);
        }

    private:
        bool mIsMainMenuShowing = false;
    };
}

#endif //CHARTIS_MAINMENUBAR_HPP
