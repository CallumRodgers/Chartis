//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_DESCRIPTORMANAGER_HPP
#define OPENMAVIS_DESCRIPTORMANAGER_HPP

#include <map>
#include <vulkan/vulkan_raii.hpp>

#include "AMORA/Scene.hpp"

#include "DescriptorAllocator.hpp"
#include "VulkanContext.hpp"
#include "Shader.hpp"

// Boost hash_combination function formula.
#define HASH_COMBINATION(A, B) (A ^ std::hash<size_t>{}(B) + 0x9e3779b9 + (A << 6) + (A >> 2))

namespace amora {
    struct BindingInfo {
        uint32_t bindingIndex;
        vk::DescriptorType bindingType;
        vk::ShaderStageFlagBits stageFlags;

        bool operator==(const BindingInfo& other) const {
            return (bindingIndex == other.bindingIndex) && (bindingType == other.bindingType) && (stageFlags == other.stageFlags);
        }
    };

    struct BindingSet {
        uint32_t setIndex;
        std::vector<BindingInfo> bindings;

        bool operator==(const BindingSet& other) const {
            if (setIndex != other.setIndex) return false;
            if (bindings.size() != other.bindings.size()) return false;
            for (uint32_t i = 0; i < bindings.size(); i++) {
                if (bindings.at(i) != other.bindings.at(i)) {
                    return false;
                }
            }
            return true;
        }
    };
}

/// Specialized hash function to identify the structure of a binding set.
template<> struct std::hash<amora::BindingSet> {
    size_t operator()(const amora::BindingSet& bindingSet) const noexcept {
        size_t result = 0;
        for (const auto& [bindingIndex, bindingType, stageFlags] : bindingSet.bindings) {
            const auto bindingHash = bindingIndex | (static_cast<size_t>(bindingType) << 8) | (static_cast<size_t>(stageFlags) << 16);
            result = HASH_COMBINATION(result, bindingHash);
        }
        return HASH_COMBINATION(result, bindingSet.setIndex);
    }
};

namespace amora {
    /**
     * The <code>DescriptorManager</code> class coordinates all the DescriptorSets, their layouts,
     * and their links to respective buffers in the whole application. The DescriptorManager object
     * creates an instance of a DescriptorAllocator per Scene, and manages shared layouts for 2D and 3D scenes.
     */
    class DescriptorLayoutManager {
    public:
        explicit DescriptorLayoutManager(const VulkanContext& context);

        void addLayout(const BindingSet& bindingSet);

        const vk::raii::DescriptorSetLayout& fetchLayout(uint32_t setIndex) const;
        const vk::raii::DescriptorSetLayout& fetchOrBuildLayout(const BindingSet& bindingSet);

    private:
        const VulkanContext& mContext;

        std::unordered_map<BindingSet, vk::raii::DescriptorSetLayout> mDescriptorSetLayoutMap;

        [[nodiscard]] vk::raii::DescriptorSetLayout createLayout(const BindingSet& bindingSet) const;
    };
}

#endif //OPENMAVIS_DESCRIPTORMANAGER_HPP
