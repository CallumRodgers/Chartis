////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include "Shader.hpp"

#include <fstream>

using namespace amora;

Shader::Shader(const vk::raii::Device& device, const std::string& shaderFileName)
    : mShaderFileName(shaderFileName.ends_with(".spv") ? shaderFileName : shaderFileName + ".spv")
{
    prepareShaderBytecode();
    const vk::ShaderModuleCreateInfo createInfo {
        .codeSize = mShaderBytecode.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>(mShaderBytecode.data()),
    };
    mModule = vk::raii::ShaderModule(device, createInfo);
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
