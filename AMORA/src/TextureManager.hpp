//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_TEXTUREMANAGER_HPP
#define OPENMAVIS_TEXTUREMANAGER_HPP

#include <unordered_map>
#include <vulkan/vulkan_raii.hpp>

#include "StaticTexture.hpp"
#include "VulkanContext.hpp"
#include "AMORA/res_id.hpp"

namespace amora {
    class TextureManager {
    public:
        TextureManager(const VulkanContext& context);

        const StaticTexture& loadStaticTexture(const CommandDispatcher& dispatcher, resource_id id, const char* filePath, bool replace = false);
        const StaticTexture& getStaticTexture(const resource_id& id);

    private:
        const VulkanContext& mContext;

        std::unordered_map<resource_id, StaticTexture> mStaticTextures;
    };
}

#endif //OPENMAVIS_TEXTUREMANAGER_HPP
