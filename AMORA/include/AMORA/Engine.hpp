//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>
#include <AMORA/Scene.hpp>

#include "ImGuiEngineData.hpp"
#include "Platform.hpp"
#include "res_id.hpp"

/**
 * Namespace of the Animated Mathematical Object Rendering Architecture (AMORA). The Vulkan code is entirely
 * isolated within the AMORA library, serving also as a backend for the Dear ImGui interface declared in
 * the main application code.
 */
namespace amora {
    ////////////////////////////////
    // BEGIN FORWARD DECLARATIONS //
    class VulkanContext;
    class RenderSurface;
    class CommandDispatcher;
    class SwapChain;
    class DescriptorLayoutManager;
    class TextureManager;
    class SemaphoreGroup;
    class FenceGroup;

    namespace graphics {
        class GraphicsPipeline;
    }
    // END FORWARD DECLARATIONS   //
    ////////////////////////////////

    /**
     * The <code>Engine</code> class is the basis of the public AMORA API. Besides the explicit
     * creation of Scenes from outside the library, all rendering management occurs through this class.
     */
    class Engine {
    public:
        struct EngineInitInfo {
            const NativePlatformInfo& platformInfo;
            uint32_t surfaceWidth;
            uint32_t surfaceHeight;
        };

        static void startLogging();
        static void endLogging();

        /** Constructs a new engine to run on a GLFW window. */
        explicit Engine(const EngineInitInfo& initInfo);

        /** Destroys any persistent Vulkan resources and stops logging. */
        ~Engine();

        /**
         * Loads the given Scene in memory. If the scene is already loaded,
         * this does nothing.
         * @param scene The scene to load.
         * @param setActive Whether the engine  should automatically switch to the new scene.
         */
        void loadScene(scene::Scene& scene, bool setActive = false);

        /**
         * Unloads the passed Scene from memory. Could happen from limited resources or
         * from closing the scene tab.
         * @param scene The scene to unload.
         */
        void unloadScene(scene::Scene& scene);

        /**
         *
         * @param newScene
         * @return The previously active scene.
         */
        scene::Scene* changeActiveScene(scene::Scene& newScene);

        /** Indicates to the engine that the swap chain is no longer valid,
         * and must be recreated. */
        void invalidateSurfaceSize(uint32_t width, uint32_t height) const;

        ImGuiEngineData beginInterfaceBackend() const;
        void endInterfaceBackend() const;

        void beginFrame();
        void endFrame();

        void waitIdle() const;

        ImTextureID loadIcon(const resource_id& id, const char* iconPath) const;

    private:
        /// Holds the Vulkan instance and devices. Automatically initialised through
        /// its trivial constructor.
        std::unique_ptr<VulkanContext> mContext;
        /// Current window surface where the application may render.
        std::unique_ptr<RenderSurface> mRenderSurface;
        /// Handles graphics-related command buffers and dispatches them into a queue.
        std::unique_ptr<CommandDispatcher> mGraphicsDispatcher;
        /// Handles compute-related command buffers and dispatches them into a queue.
        std::unique_ptr<CommandDispatcher> mComputeDispatcher;
        /// The set of images that get drawn onto and presented to the screen.
        std::unique_ptr<SwapChain> mSwapChain;
        /// Handles the descriptors across the application, i.e. the shader bindings, their link
        /// with data buffers, etc.
        std::unique_ptr<DescriptorLayoutManager> mDescriptorManager;
        /// Manages and owns all the global textures used by Chartis.
        std::unique_ptr<TextureManager> mTextureManager;

        std::unique_ptr<SemaphoreGroup> mPresentCompleteSemaphores;
        std::unique_ptr<SemaphoreGroup> mRenderCompleteSemaphores;
        std::unique_ptr<FenceGroup> mDrawFences;

        uint32_t mFrameIndex = 0;
        uint32_t mImageIndex = 0;

        std::vector<scene::Scene*> mLoadedScenes;
        scene::Scene* mActiveScene = nullptr;

        void terminate() const;

    };
}

#endif // ENGINE_HPP
