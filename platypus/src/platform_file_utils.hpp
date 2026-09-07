////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_PLATFORM_FILE_UTILS_HPP
#define CHARTIS_PLATFORM_FILE_UTILS_HPP

#include <string>
#include <tuple>

#include <stb_image.h>

namespace chartis::platypus::file_utils {
    static std::tuple<int, int, unsigned char*> loadImage(const std::string& filePath) {
        int width, height, channels;
        stbi_uc* data = stbi_load(filePath.data(), &width, &height, &channels, 4);
        return {width, height, data};
    }

    static void freeImage(stbi_uc* data) {
        stbi_image_free(data);
    }
}

#endif //CHARTIS_PLATFORM_FILE_UTILS_HPP
