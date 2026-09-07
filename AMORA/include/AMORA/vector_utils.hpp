////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#ifndef CHARTIS_VECTOR_UTILS_HPP
#define CHARTIS_VECTOR_UTILS_HPP

#include <vector>

namespace amora::vector_utils {
    template <typename T> static bool contains(const std::vector<T>& vector, const T& obj) {
        for (T member : vector) {
            if (member == obj) {
                return true;
            }
        }
        return false;
    }

    template <typename T> static int indexOf(const std::vector<T>& vector, const T& element) {
        for (int index = 0; index < vector.size(); index++) {
            if (vector[index] == element) {
                return index;
            }
        }
        return -1;
    }

    template <typename K, typename T> static std::vector<T> fromUnorderedMapValues(const std::unordered_map<K, T>& map) {
        std::vector<T> vector;
        for (const std::pair<K, T>& pair : map) {
            vector.push_back(pair.second);
        }
        return vector;
    }
}

#endif //CHARTIS_VECTOR_UTILS_HPP
