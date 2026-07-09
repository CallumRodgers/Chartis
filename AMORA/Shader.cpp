////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

module;

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
#include <vulkan/vulkan_raii.hpp>

#include <fstream>
#include <vector>

module amora;

using namespace AMORA;

Shader::Shader(const std::string &shaderFileName) {
    mShaderFileName = shaderFileName;
    prepareShaderBytecode();
}

vk::raii::ShaderModule Shader::getModule(const vk::raii::Device &device) const {
    const vk::ShaderModuleCreateInfo createInfo {
        .codeSize = mShaderBytecode.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>(mShaderBytecode.data()),
    };
    return vk::raii::ShaderModule(device, createInfo);
}

void Shader::prepareShaderBytecode() {
    std::ifstream shaderFile(mShaderFileName, std::ios::ate | std::ios::binary);

    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + mShaderFileName);
    }

    std::vector<char> buffer(shaderFile.tellg());

    shaderFile.seekg(0, std::ios::beg);
    shaderFile.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    shaderFile.close();

    mShaderBytecode = buffer;
}
