////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GLFW/glfw3.h>
#include <AMORA/Scene.hpp>
#include <memory>

/**
 * Namespace of the Animated Mathematical Object Rendering Architecture (AMORA). The Vulkan code is entirely
 * isolated within the AMORA library, serving also as a backend for the Dear ImGui interface declared in
 * the main application code.
 */
namespace AMORA {
    // =====================
    // Forward declarations.
    // =====================

    class VulkanContext;
    class RenderSurface;
    class CommandDispatcher;
    class SwapChain;
    class SemaphoreGroup;
    class FenceGroup;

    namespace Graphics {
        class GraphicsPipeline;
    }

    /**
     * The <code>Engine</code> class is the basis of the public AMORA API. Besides the explicit
     * creation of Scenes from outside the library, all rendering management occurs through this class.
     */
    class Engine {
    public:
        static void startLogging();
        static void endLogging();

        /** Constructs a new engine to run on a GLFW window. */
        explicit Engine(GLFWwindow* window);

        /** Destroys any persistent Vulkan resources and stops logging. */
        ~Engine();

        /**
         * Loads the given Scene in memory. If the scene is already loaded,
         * this does nothing.
         * @param scene The scene to load.
         */
        void loadScene(const Scene& scene) const;

        /**
         * Unloads the passed Scene from memory. Could happen from limited resources or
         * from closing the scene tab.
         * @param scene The scene to unload.
         */
        void unloadScene(const Scene& scene) const;

        /**
         *
         * @param newScene
         * @return The previously active scene.
         */
        const Scene& changeActiveScene(const Scene& newScene);

        /** Renders a frame of the currently active Scene to the swap chain. */
        void renderActive();

        /** Indicates to the engine that the swap chain is no longer valid,
         * and must be recreated. */
        void invalidateSwapChain() { mSwapChainInvalidated = true; };

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

        std::unique_ptr<SemaphoreGroup> mPresentCompleteSemaphores;
        std::unique_ptr<SemaphoreGroup> mRenderCompleteSemaphores;
        std::unique_ptr<FenceGroup> mDrawFences;

        /// Temporary pipeline.
        std::unique_ptr<Graphics::GraphicsPipeline> mPipeline;

        uint32_t mFrameIndex = 0;

        bool mSwapChainInvalidated = false;

        std::vector<Scene> mLoadedScenes;

        void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex) const;

        void terminate() const;

    };
}

#endif // ENGINE_HPP
