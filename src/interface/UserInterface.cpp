//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "UserInterface.hpp"

#include <imgui.h>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "MainBar.hpp"
#include "SideBar.hpp"

using namespace chartis::interface;

static bool resetLayout = true;

UserInterface::UserInterface(const amora::ImGuiEngineData& data, const IconManager& iconManager) : mIconManager(iconManager) {
    ImGui::SetAllocatorFunctions(data.AllocFunc, data.FreeFunc, data.AllocUserData);
    ImGui::SetCurrentContext(data.Context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    const float scale = 1.0f;
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
    style.WindowRounding = 10.0f;
    style.DisplaySafeAreaPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(8.f, 8.f);

    style.FontSizeBase = 11.0f;
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf");
    ImGui::PushFont(robotoFont);

    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    ImGui::ClearIniSettings();
}

void UserInterface::layout(ChartisApplication* app) {
    const UIContext context {
        .app = app,
        .iconManager = mIconManager
    };

    mMainBar.layout(context);

    ///////////////////////////
    // SETTING UP DOCK SPACE //
    ///////////////////////////

    const ImGuiID dockSpaceID = ImGui::GetID("Chartis DockSpace");
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dockSpaceID) == nullptr || resetLayout) {
        resetLayout = false;

        ImGui::DockBuilderAddNode(dockSpaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpaceID, viewport->Size);
        ImGuiID dock_id_left = 0;
        ImGuiID dock_id_main = dockSpaceID;

        const float sideBarRatio = SideBar::WIDTH / viewport->WorkSize.x;

        ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, sideBarRatio, &dock_id_left, &dock_id_main);

        ImGui::DockBuilderDockWindow("Main", dock_id_main);
        ImGui::DockBuilderDockWindow(SideBar::WINDOW_NAME, dock_id_left);
        ImGui::DockBuilderFinish(dockSpaceID);
    }

    const auto dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | static_cast<ImGuiDockNodeFlags_>(ImGuiDockNodeFlags_NoTabBar) | ImGuiDockNodeFlags_NoResize;
    ImGui::DockSpaceOverViewport(dockSpaceID, viewport, dockSpaceFlags);




    //ImGui::ShowDemoWindow();

    ////////////////////////////
    // SETTING UP SIDE WINDOW //
    ////////////////////////////

    mSideBar.layout(context);
}
