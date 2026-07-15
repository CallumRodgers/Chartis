////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <cstring>

#include <AMORA/Config.hpp>
#include <AMORA/Logger.hpp>
#include "VulkanContext.hpp"

using namespace AMORA;

/**
 * Retrieves all the required GLFW extensions plus any extra ones
 * OpenMaVis needs.
 * @return a vector with
 */
std::vector<const char*> getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}

std::vector<const char*> getRequiredInstanceLayers() {
    // Get the required layers
    std::vector<char const*> requiredLayers;
    if (Config::Layers::ENABLE_VALIDATION_LAYERS) {
        requiredLayers.assign(Config::Layers::VALIDATION_LAYERS.begin(), Config::Layers::VALIDATION_LAYERS.end());
    }
    return requiredLayers;
}

void checkRequiredExtensionsAvailability(const std::vector<const char*>& requiredExtensions,
    const std::vector<vk::ExtensionProperties>& extensionProperties) {
    const auto unsupportedExtensionIt = std::ranges::find_if(
        requiredExtensions, [&extensionProperties](auto const &requiredExtension){
            return std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) {
                return std::strcmp(extensionProperty.extensionName, requiredExtension) == 0;
            });
        }
    );
    if (unsupportedExtensionIt != requiredExtensions.end()) {
        throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedExtensionIt));
    }
}

void checkRequiredLayersAvailability(const std::vector<const char*>& requiredLayers,
    const std::vector<vk::LayerProperties>& layerProperties) {
    const auto unsupportedLayerIt = std::ranges::find_if(
        requiredLayers, [&layerProperties](auto const &requiredLayer) {
            return std::ranges::none_of(layerProperties, [requiredLayer](auto const &layerProperty) {
                return std::strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
        }
    );
    if (unsupportedLayerIt != requiredLayers.end()) {
        throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
    }
}

VulkanContext::VulkanContext() {
    vk::ApplicationInfo appInfo = {
        .pApplicationName   = Config::Info::APPLICATION_NAME,
        .applicationVersion = Config::Info::APPLICATION_VERSION,
        .pEngineName        = Config::Info::ENGINE_NAME,
        .engineVersion      = Config::Info::ENGINE_VERSION,
        .apiVersion         = vk::ApiVersion14
    };

    auto requiredExtensions = getRequiredInstanceExtensions();
    const auto extensionProperties = mContext.enumerateInstanceExtensionProperties();

    // Throws an error if a required extension is not present.
    checkRequiredExtensionsAvailability(requiredExtensions, extensionProperties);

    auto requiredLayers = getRequiredInstanceLayers();
    const auto layerProperties = mContext.enumerateInstanceLayerProperties();

    // Throws an error if a required layer is not present.
    checkRequiredLayersAvailability(requiredLayers, layerProperties);

    const vk::InstanceCreateInfo createInfo = {
        .pApplicationInfo           = &appInfo,
        .enabledLayerCount          = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames        = requiredLayers.data(),
        .enabledExtensionCount      = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames    = requiredExtensions.data(),
    };

    mInstance = vk::raii::Instance(mContext, createInfo);
}

void VulkanContext::setupDevices(const vk::raii::SurfaceKHR& surface) {
    pickPhysicalDevice();
    createLogicalDevice(surface);
}

void VulkanContext::pickPhysicalDevice() {
    const auto physicalDevices = mInstance.enumeratePhysicalDevices();

    if (physicalDevices.empty()) {
        throw std::runtime_error("No physical devices with Vulkan support found.");
    }

    std::vector<vk::raii::PhysicalDevice> capableDevices;

    // Checking capabilities of listed devices.
    for (auto const &physicalDevice : physicalDevices) {
        auto properties = physicalDevice.getProperties();
        auto features = physicalDevice.getFeatures();

        if (!features.geometryShader || properties.apiVersion < vk::ApiVersion13) {
            continue;
        }

        // Checking support for graphics and compute queues.
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        bool supportsGraphicsQueue = std::ranges::any_of(queueFamilies, [](auto const &queueFamily) {
            return !!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics);
        });
        bool supportsComputeQueue = std::ranges::any_of(queueFamilies, [](auto const &queueFamily) {
            return !!(queueFamily.queueFlags & vk::QueueFlagBits::eCompute);
        });

        if (!supportsGraphicsQueue || !supportsComputeQueue) {
            continue;
        }

        capableDevices.push_back(physicalDevice);
    }

    if (capableDevices.empty()) {
        throw std::runtime_error("No devices with Vulkan support meet requirements.");
    }

    if (capableDevices.size() == 1) {
        const auto device = capableDevices.at(0);
        ENGINE_INFO("Automatically selected physical device: {}", std::string_view(device.getProperties().deviceName.data()));
        mPhysicalDevice = device;
    } else {
        std::cout << "Found " << physicalDevices.size() << " devices with Vulkan support:" << std::endl;
        for (int i = 0; i < physicalDevices.size(); i++) {
            auto deviceName = physicalDevices.at(i).getProperties().deviceName;
            std::cout << i << ". " << deviceName << std::endl;
        }

        int selection;
        while (true) {
            try {
                std::cout << "Select device by number : ";
                std::string selectedDevice;
                std::cin >> selectedDevice;
                selection = std::stoi(selectedDevice);
                break;
            } catch (const std::exception &e) {
                std::cout << "Invalid input." << std::endl;
            }
        }

        mPhysicalDevice = physicalDevices.at(selection);
    }
}

void VulkanContext::createLogicalDevice(const vk::raii::SurfaceKHR& surface) {
    auto queueFamilies = mPhysicalDevice.getQueueFamilyProperties();

    uint32_t graphicsQueueIndex = VK_QUEUE_FAMILY_IGNORED;
    uint32_t computeQueueIndex = VK_QUEUE_FAMILY_IGNORED;

    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        auto queueFamily = queueFamilies.at(i);

        // Checking for a graphics queue.
        if ((queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0)) {
            if (mPhysicalDevice.getSurfaceSupportKHR(i, *surface)) {
                graphicsQueueIndex = i;
            }
        }

        if ((queueFamily.queueFlags & vk::QueueFlagBits::eCompute) != static_cast<vk::QueueFlags>(0)) {
            computeQueueIndex = i;
        }
    }

    if (graphicsQueueIndex == VK_QUEUE_FAMILY_IGNORED) {
        throw std::runtime_error("No graphics queue with presentation support found.");
    }
    if (computeQueueIndex == VK_QUEUE_FAMILY_IGNORED) {
        throw std::runtime_error("No compute queue family found.");
    }

    const vk::DeviceQueueCreateInfo graphicsQueueCreateInfo {
        .queueFamilyIndex = graphicsQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &Config::Rendering::GRAPHICS_QUEUE_PRIORITY,
    };
    const vk::DeviceQueueCreateInfo computeQueueCreateInfo {
        .queueFamilyIndex = computeQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &Config::Rendering::COMPUTE_QUEUE_PRIORITY
    };

    const vk::DeviceQueueCreateInfo queueCreateInfos[] = {graphicsQueueCreateInfo, computeQueueCreateInfo};

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan11Features,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;

    auto& vulkan11 = featureChain.get<vk::PhysicalDeviceVulkan11Features>();
    vulkan11.shaderDrawParameters = true;

    auto& vulkan13 = featureChain.get<vk::PhysicalDeviceVulkan13Features>();
    vulkan13.dynamicRendering = true;
    vulkan13.synchronization2 = true;

    auto& extendedStateEXT = featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    extendedStateEXT.extendedDynamicState = true;

    auto extensions = Config::Extensions::REQUIRED_DEVICE_EXTENSIONS;

    vk::DeviceCreateInfo deviceCreateInfo {
        .pNext                      = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount       = 1,
        .pQueueCreateInfos          = queueCreateInfos,
        .enabledExtensionCount      = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames    = extensions.data()
    };

    mDevice = vk::raii::Device(mPhysicalDevice, deviceCreateInfo);

    mGraphicsIndex = graphicsQueueIndex;
    mComputeIndex = computeQueueIndex;
}
