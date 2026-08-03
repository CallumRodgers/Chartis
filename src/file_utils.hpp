//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_FILE_UTILS_HPP
#define OPENMAVIS_FILE_UTILS_HPP

#include <stb_image.h>
#include <string>

namespace mavis::file_utils {
    static std::tuple<int, int, unsigned char*> loadImage(const std::string& filePath) {
        int width, height, channels;
        stbi_uc* data = stbi_load(filePath.data(), &width, &height, &channels, 4);
        return {width, height, data};
    }

    static void freeImage(stbi_uc* data) {
        stbi_image_free(data);
    }
}

#endif //OPENMAVIS_FILE_UTILS_HPP
