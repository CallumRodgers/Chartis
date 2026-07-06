////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
// Full license text available in the LICENSE file at the root of this repository.
////////////////////////////////////////////////////////////////////////////////

module;

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

module amora;

using namespace AMORA;

Engine::Engine() {}

void Engine::initialise(GLFWwindow* window) {
    createInstance();
    createSurface(window);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain(window);
    std::cout << "Successfully created swap chain." << std::endl;
    createImageViews();
    createGraphicsPipeline();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

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
    if (enableValidationLayers) {
        requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
    }
    return requiredLayers;
}

void checkRequiredExtensionsAvailability(std::vector<const char*> const& requiredExtensions,
    std::vector<vk::ExtensionProperties> const& extensionProperties) {
    const auto unsupportedExtensionIt = std::ranges::find_if(
        requiredExtensions, [&extensionProperties](auto const &requiredExtension){
            return std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) {
                return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
            });
        }
    );
    if (unsupportedExtensionIt != requiredExtensions.end()) {
        throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedExtensionIt));
    }
}

void checkRequiredLayersAvailability(std::vector<const char*> const& requiredLayers,
    std::vector<vk::LayerProperties> const& layerProperties) {
    const auto unsupportedLayerIt = std::ranges::find_if(
        requiredLayers, [&layerProperties](auto const &requiredLayer) {
            return std::ranges::none_of(layerProperties, [requiredLayer](auto const &layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
        }
    );
    if (unsupportedLayerIt != requiredLayers.end()) {
        throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
    }
}

void Engine::createInstance() {
    vk::ApplicationInfo appInfo = {
        .pApplicationName = "OpenMaVis",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "AMORA Engine",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = vk::ApiVersion14
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
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    mInstance = vk::raii::Instance(mContext, createInfo);
}

void Engine::createSurface(GLFWwindow* window) {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*mInstance, window, nullptr, &_surface) != 0) {
        throw std::runtime_error("Failed to create window surface.");
    }
    mSurface = vk::raii::SurfaceKHR(mInstance, _surface);
}

void Engine::pickPhysicalDevice() {
    auto physicalDevices = mInstance.enumeratePhysicalDevices();

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
        std::cout << "Automatically selected physical device: " << device.getProperties().deviceName << std::endl;
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

void Engine::createLogicalDevice() {
    auto queueFamilies = mPhysicalDevice.getQueueFamilyProperties();

    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        auto queueFamily = queueFamilies.at(i);

        // Checking for a graphics queue.
        if ((queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0)) {
            if (mPhysicalDevice.getSurfaceSupportKHR(i, *mSurface)) {
                mGraphicsQueueIndex = i;
            }
        }

        if ((queueFamily.queueFlags & vk::QueueFlagBits::eCompute) != static_cast<vk::QueueFlags>(0)) {
            mComputeQueueIndex = i;
        }
    }

    if (mGraphicsQueueIndex == VK_QUEUE_FAMILY_IGNORED) {
        throw std::runtime_error("No graphics queue with presentation support found.");
    }
    if (mComputeQueueIndex == VK_QUEUE_FAMILY_IGNORED) {
        throw std::runtime_error("No compute queue family found.");
    }

    vk::DeviceQueueCreateInfo graphicsQueueCreateInfo {
        .queueFamilyIndex = mGraphicsQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &GRAPHICS_QUEUE_PRIORITY,
    };
    vk::DeviceQueueCreateInfo computeQueueCreateInfo {
        .queueFamilyIndex = mComputeQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &COMPUTE_QUEUE_PRIORITY
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

    vk::DeviceCreateInfo deviceCreateInfo {
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = queueCreateInfos,
        .enabledExtensionCount = static_cast<uint32_t>(REQUIRED_DEVICE_EXTENSIONS.size()),
        .ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data()
    };

    mDevice = vk::raii::Device(mPhysicalDevice, deviceCreateInfo);

    mGraphicsQueue = vk::raii::Queue(mDevice, mGraphicsQueueIndex, 0);
    mComputeQueue = vk::raii::Queue(mDevice, mComputeQueueIndex, 0);
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats) {
    const auto formatIt = std::ranges::find_if(
            availableFormats,
            [](const auto &format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes) {
    bool supportsFIFO = false;
    bool supportsMailbox = false;

    // Checking for FIFO and Mailbox modes.
    std::ranges::for_each(availablePresentModes, [&supportsMailbox, &supportsFIFO](auto presentMode) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            supportsMailbox = true;
        } else if (presentMode == vk::PresentModeKHR::eFifo) {
            supportsFIFO = true;
        }
    });

    // All graphics cards should support FIFO mode.
    assert(supportsFIFO);
    return supportsMailbox ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(GLFWwindow* window, vk::SurfaceCapabilitiesKHR const &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount)) {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

void Engine::createSwapChain(GLFWwindow* window) {
    auto surfaceCapabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(*mSurface);
    mSwapChainExtent = chooseSwapExtent(window, surfaceCapabilities);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    const auto availableFormats = mPhysicalDevice.getSurfaceFormatsKHR(*mSurface);
    mSwapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    const auto availablePresentModes = mPhysicalDevice.getSurfacePresentModesKHR(*mSurface);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo = {
                    .surface          = *mSurface,
                    .minImageCount    = minImageCount,
                    .imageFormat      = mSwapChainSurfaceFormat.format,
                    .imageColorSpace  = mSwapChainSurfaceFormat.colorSpace,
                    .imageExtent      = mSwapChainExtent,
                    .imageArrayLayers = 1,
                    .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                    .imageSharingMode = vk::SharingMode::eExclusive,
                    .preTransform     = surfaceCapabilities.currentTransform,
                    .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                    .presentMode      = chooseSwapPresentMode(availablePresentModes),
                    .clipped          = true
    };

    mSwapChain = vk::raii::SwapchainKHR(mDevice, swapChainCreateInfo);
    mSwapChainImages = mSwapChain.getImages();
}

void Engine::createImageViews() {
    assert(mSwapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo = {
        .viewType = vk::ImageViewType::e2D,
        .format = mSwapChainSurfaceFormat.format,
        .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1}
    };

    for (const auto &image : mSwapChainImages) {
        imageViewCreateInfo.image = image;
        mSwapChainImageViews.emplace_back(mDevice, imageViewCreateInfo);
    }
}

void Engine::createGraphicsPipeline() {
    const auto shaderModule = Shader("shader.spv").getModule(mDevice);

    // Creating vertex shader info.
    vk::PipelineShaderStageCreateInfo vertStageInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule,
        .pName = SHADER_ENTRY_VERTEX
    };

    // Now fragment.
    vk::PipelineShaderStageCreateInfo fragStageInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModule,
        .pName = SHADER_ENTRY_FRAGMENT
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

    std::vector dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };
    vk::PipelineViewportStateCreateInfo viewportStateInfo {
        .viewportCount = 1, .scissorCount = 1
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer {
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = vk::PolygonMode::eFill,
        .cullMode                = vk::CullModeFlagBits::eBack,
        .frontFace               = vk::FrontFace::eClockwise,
        .depthBiasEnable         = vk::False,
        .lineWidth               = 1.0f
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment {
        .blendEnable         = vk::True,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp        = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp        = vk::BlendOp::eAdd,
        .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };
    vk::PipelineColorBlendStateCreateInfo colorBlending {
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo {.setLayoutCount = 0, .pushConstantRangeCount = 0};
    mPipelineLayout = vk::raii::PipelineLayout(mDevice, pipelineLayoutInfo);

    vk::PipelineMultisampleStateCreateInfo multisampleInfo {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {.stageCount          = 2,
         .pStages             = shaderStages,
         .pVertexInputState   = &vertexInputInfo,
         .pInputAssemblyState = &inputAssemblyInfo,
         .pViewportState      = &viewportStateInfo,
         .pRasterizationState = &rasterizer,
         .pMultisampleState   = &multisampleInfo,
         .pColorBlendState    = &colorBlending,
         .pDynamicState       = &dynamicStateInfo,
         .layout              = mPipelineLayout,
         .renderPass          = nullptr},

        {.colorAttachmentCount = 1, .pColorAttachmentFormats = &mSwapChainSurfaceFormat.format}};

    mPipeline = vk::raii::Pipeline(mDevice, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

void Engine::createCommandPool() {
    const vk::CommandPoolCreateInfo commandPoolInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = mGraphicsQueueIndex
    };
    mCommandPool = vk::raii::CommandPool(mDevice, commandPoolInfo);
}

void Engine::createCommandBuffer() {
    const vk::CommandBufferAllocateInfo bufferInfo {
        .commandPool = mCommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    mCommandBuffer = std::move(vk::raii::CommandBuffers(mDevice, bufferInfo).front());
}

void Engine::transitionImageLayout(
        uint32_t                imageIndex,
        vk::ImageLayout         old_layout,
        vk::ImageLayout         new_layout,
        vk::AccessFlags2        src_access_mask,
        vk::AccessFlags2        dst_access_mask,
        vk::PipelineStageFlags2 src_stage_mask,
        vk::PipelineStageFlags2 dst_stage_mask) {
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask        = src_stage_mask,
        .srcAccessMask       = src_access_mask,
        .dstStageMask        = dst_stage_mask,
        .dstAccessMask       = dst_access_mask,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = mSwapChainImages[imageIndex],
        .subresourceRange    = {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1}};
    vk::DependencyInfo dependency_info = {
        .dependencyFlags         = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier};
    mCommandBuffer.pipelineBarrier2(dependency_info);
}

void Engine::recordCommandBuffer(uint32_t imageIndex) {

    mCommandBuffer.begin({});

    transitionImageLayout(
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput
    );

    constexpr vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo {
        .imageView   = mSwapChainImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = clearColor
    };

    const vk::RenderingInfo renderInfo {
        .renderArea = {.offset = {0,0}, .extent = mSwapChainExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    mCommandBuffer.beginRendering(renderInfo);

    mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    mCommandBuffer.setViewport(0, vk::Viewport(
        0.0f, 0.0f,
        static_cast<float>(mSwapChainExtent.width), static_cast<float>(mSwapChainExtent.height),
        0.0f, 1.0f
    ));
    mCommandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), mSwapChainExtent));

    mCommandBuffer.draw(3, 1, 0, 0);

    mCommandBuffer.endRendering();

    transitionImageLayout(
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
        {},                                                     // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
    );

    mCommandBuffer.end();
}

void Engine::createSyncObjects() {
    mPresentCompleteSemaphore = vk::raii::Semaphore(mDevice, vk::SemaphoreCreateInfo());
    mRenderCompleteSemaphore = vk::raii::Semaphore(mDevice, vk::SemaphoreCreateInfo());
    mDrawFence = vk::raii::Fence(mDevice, {.flags = vk::FenceCreateFlagBits::eSignaled});
}

void Engine::drawFrame() {
    auto fenceResult = mDevice.waitForFences(*mDrawFence, vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fences!");
    }

    mDevice.resetFences(*mDrawFence);

    auto [result, imageIndex] = mSwapChain.acquireNextImage(UINT64_MAX, *mPresentCompleteSemaphore, nullptr);

    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo {
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &*mPresentCompleteSemaphore,
        .pWaitDstStageMask    = &waitDestinationStageMask,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &*mCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &*mRenderCompleteSemaphore
    };

    mGraphicsQueue.submit(submitInfo, *mDrawFence);

    const vk::PresentInfoKHR presentInfo {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &*mRenderCompleteSemaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &*mSwapChain,
        .pImageIndices      = &imageIndex
    };

    result = mGraphicsQueue.presentKHR(presentInfo);
}

void Engine::terminate() {
    mDevice.waitIdle();
}
