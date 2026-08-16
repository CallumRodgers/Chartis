//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef OPENMAVIS_DP_HPP
#define OPENMAVIS_DP_HPP

namespace mavis::interface {
    class dp {
    public:
        float value;

        static inline float scale_factor = 1.0f;

        static void set_scale(float scale) {
            scale_factor = scale;
        }

        constexpr explicit dp(float v) : value(v) {}

        operator float() const {
            return value * scale_factor;
        }

        dp operator+(dp other) const { return dp(value + other.value); }
        dp operator-(dp other) const { return dp(value - other.value); }
        dp operator*(float scalar) const { return dp(value * scalar); }
        dp operator/(float scalar) const { return dp(value / scalar); }
    };

    constexpr dp operator""_dp(long double val) {
        return dp(static_cast<float>(val));
    }

    constexpr dp operator""_dp(unsigned long long val) {
        return dp(static_cast<float>(val));
    }
}

#endif //OPENMAVIS_DP_HPP
