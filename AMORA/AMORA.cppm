////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
// Full license text available in the LICENSE file at the root of this repository.
////////////////////////////////////////////////////////////////////////////////

module;

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

export module amora;

const std::vector VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector REQUIRED_DEVICE_EXTENSIONS = {
    vk::KHRSwapchainExtensionName
};

constexpr float GRAPHICS_QUEUE_PRIORITY = 0.6f;
constexpr float COMPUTE_QUEUE_PRIORITY = 0.4f;

#if NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr auto SHADER_ENTRY_VERTEX = "vertMain";
constexpr auto SHADER_ENTRY_TESSELATION = "tessMain";
constexpr auto SHADER_ENTRY_FRAGMENT = "fragMain";
constexpr auto SHADER_ENTRY_COMPUTE = "compMain";

export namespace AMORA {

    class Engine {
    public:
        Engine();

        void initialise(GLFWwindow* window);

        void drawFrame();

        void terminate();

    private:
        vk::raii::Context mContext;
        vk::raii::Instance mInstance = nullptr;
        vk::raii::SurfaceKHR mSurface = nullptr;
        vk::raii::PhysicalDevice mPhysicalDevice = nullptr;
        vk::raii::Device mDevice = nullptr;

        uint32_t mGraphicsQueueIndex = VK_QUEUE_FAMILY_IGNORED;
        uint32_t mComputeQueueIndex = VK_QUEUE_FAMILY_IGNORED;
        vk::raii::Queue mGraphicsQueue = nullptr;
        vk::raii::Queue mComputeQueue = nullptr;

        vk::raii::SwapchainKHR mSwapChain = nullptr;
        std::vector<vk::Image> mSwapChainImages;
        std::vector<vk::raii::ImageView> mSwapChainImageViews;
        vk::SurfaceFormatKHR mSwapChainSurfaceFormat;
        vk::Extent2D mSwapChainExtent;

        vk::raii::PipelineLayout mPipelineLayout = nullptr;
        vk::raii::Pipeline mPipeline = nullptr;

        vk::raii::CommandPool mCommandPool = nullptr;
        vk::raii::CommandBuffer mCommandBuffer = nullptr;

        vk::raii::Semaphore mPresentCompleteSemaphore = nullptr;
        vk::raii::Semaphore mRenderCompleteSemaphore = nullptr;
        vk::raii::Fence mDrawFence = nullptr;

        void createInstance();
        void createSurface(GLFWwindow* window);
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain(GLFWwindow* window);
        void createImageViews();
        void createGraphicsPipeline();
        void createCommandPool();
        void createCommandBuffer();
        void createSyncObjects();

        void transitionImageLayout(
                uint32_t                imageIndex,
                vk::ImageLayout         old_layout,
                vk::ImageLayout         new_layout,
                vk::AccessFlags2        src_access_mask,
                vk::AccessFlags2        dst_access_mask,
                vk::PipelineStageFlags2 src_stage_mask,
                vk::PipelineStageFlags2 dst_stage_mask
                );
        void recordCommandBuffer(uint32_t imageIndex);
    };

    class Pipeline {
    public:
        Pipeline() = default;

    private:
    };

    class Shader {
    public:
        explicit Shader(const std::string &shaderFileName);

        [[nodiscard]] vk::raii::ShaderModule getModule(const vk::raii::Device &device) const;

    private:
        std::string mShaderFileName;
        std::vector<char> mShaderBytecode;

        void prepareShaderBytecode();
    };
}
