////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef OPENMAVIS_SHADER_HPP
#define OPENMAVIS_SHADER_HPP

#include <vulkan/vulkan_raii.hpp>

namespace AMORA {
    enum ShaderStage {
        VERTEX,
        TESSELATION,
        FRAGMENT,
        COMPUTE
    };

    /// Note that while Slang allows naming the entry something else,
    /// when converted to SPIR-V the main function always gets renamed to "main".
    constexpr auto SHADER_ENTRY_POINT = "main";

    /**
     * A <code>Shader</code> object in OpenMaVis is directly equivalent to a shader stage.
     * While Slang supports including multiple stages within a single file, the logic
     * for pipeline creation is just easier when 1 stage = 1 file, and it enables
     * easy modification for pipeline creation from user-created fragment shaders.
     */
    class Shader {
    public:
        Shader(const vk::raii::Device& device, const std::string& shaderFileName);

        [[nodiscard]] const vk::raii::ShaderModule& getModule() const noexcept { return mModule; }

    private:
        std::string mShaderFileName;
        std::vector<char> mShaderBytecode;

        vk::raii::ShaderModule mModule = nullptr;

        void prepareShaderBytecode();
    };
}

#endif //OPENMAVIS_SHADER_HPP
