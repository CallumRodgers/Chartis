////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "DescriptorLayoutManager.hpp"

#include "AMORA/logger.hpp"

using namespace amora;

DescriptorLayoutManager::DescriptorLayoutManager(const VulkanContext& context) : mContext(context) {

}

void DescriptorLayoutManager::addLayout(const BindingSet& bindingSet) {
    mDescriptorSetLayoutMap.try_emplace(bindingSet, createLayout(bindingSet));
}

const vk::raii::DescriptorSetLayout& DescriptorLayoutManager::fetchLayout(const uint32_t setIndex) const {
    for (auto& [set, layout] : mDescriptorSetLayoutMap) {
        if (set.setIndex == setIndex) {
            return layout;
        }
    }
    throw std::invalid_argument("DescriptorSetLayout for set index '" + std::to_string(setIndex) + "' has not been created.");
}

const vk::raii::DescriptorSetLayout& DescriptorLayoutManager::fetchOrBuildLayout(const BindingSet& bindingSet) {
    if (!mDescriptorSetLayoutMap.contains(bindingSet)) {
        mDescriptorSetLayoutMap.emplace(bindingSet, createLayout(bindingSet));
    }
    return mDescriptorSetLayoutMap.at(bindingSet);
}

vk::raii::DescriptorSetLayout DescriptorLayoutManager::createLayout(const BindingSet& bindingSet) const {
    /// Vector to hold the bindings this set has.
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (const auto& [bindingIndex, bindingType, stageFlags] : bindingSet.bindings) {
        bindings.push_back({
            .binding = bindingIndex,
            .descriptorType = bindingType,
            .descriptorCount = 1,
            .stageFlags = stageFlags
        });
    }

    // If there are no bindings this descriptor set is useless and
    // shouldn't be created.
    assert(!bindings.empty());

    const vk::DescriptorSetLayoutCreateInfo layoutCreateInfo {
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };
    return std::move(vk::raii::DescriptorSetLayout(mContext.getDevice(), layoutCreateInfo));
}
