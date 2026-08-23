//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef CHARTIS_SHADER_HPP
#define CHARTIS_SHADER_HPP

#include <map>
#include <AMORA/config.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace amora {
    enum ShaderStage {
        Vertex,
        TesselationControl,
        TesselationEvaluation,
        Fragment,
        Compute
    };

    /// Note that while Slang allows naming the entry something else,
    /// when converted to SPIR-V the main function always gets renamed to "main".
    constexpr auto SHADER_ENTRY_POINT = "main";

    /**
     * A <code>Shader</code> object in Chartis is directly equivalent to a shader stage.
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

        vk::raii::ShaderModule mModule {nullptr};

        void prepareShaderBytecode();
    };

    /**
     * A ShaderMap is a std::map that associates a Shader object with its respective
     * pipeline stage, like Vertex, Fragment, Compute, etc. Maps for graphics pipelines
     * <b>must</b> provide valid Vertex and Fragment shader objects, otherwise an error
     * will be thrown. Tesselation shaders are optional.
     */
    typedef std::map<ShaderStage, std::shared_ptr<Shader> > ShaderMap;
}

#endif //CHARTIS_SHADER_HPP
