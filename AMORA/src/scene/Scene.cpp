//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include "AMORA/Scene.hpp"

#include "scene/Background.hpp"
#include "scene/CoordinateGrid.hpp"
#include "DescriptorAllocator.hpp"
#include "DescriptorSetData.hpp"
#include "UniformBuffer.hpp"

using namespace amora::scene;

Scene::~Scene() = default;
