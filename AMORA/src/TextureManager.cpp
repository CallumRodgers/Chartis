////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "TextureManager.hpp"

#include "AMORA/file_utils.hpp"

using namespace amora;

TextureManager::TextureManager(const VulkanContext& context) : mContext(context) {}

const StaticTexture& TextureManager::loadStaticTexture(const CommandDispatcher& dispatcher, resource_id id, const char* filePath, bool replace) {
    auto [width, height, pixels] = file_utils::loadImage(filePath);
    if (replace) {
        if (mStaticTextures.contains(id)) {
            mStaticTextures.erase(id);
            mStaticTextures.emplace(id, StaticTexture(mContext, width, height, pixels));
        }
    } else {
        mStaticTextures.try_emplace(id, StaticTexture(mContext, width, height, pixels));
    }
    file_utils::freeImage(pixels);

    StaticTexture& texture = mStaticTextures.at(id);
    texture.prepare(dispatcher);

    return texture;
}

const StaticTexture& TextureManager::getStaticTexture(const resource_id& id) {
    if (mStaticTextures.contains(id)) {
        return mStaticTextures.at(id);
    }
    throw std::invalid_argument("No texture was found with id: '" + id + "'.");
}
