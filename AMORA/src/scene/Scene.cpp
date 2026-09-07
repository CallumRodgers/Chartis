////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "AMORA/Scene.hpp"

#include "scene/Background.hpp"
#include "scene/CoordinateGrid.hpp"
#include "DescriptorAllocator.hpp"
#include "DescriptorSetData.hpp"
#include "UniformBuffer.hpp"

using namespace amora::scene;

Scene::~Scene() = default;
