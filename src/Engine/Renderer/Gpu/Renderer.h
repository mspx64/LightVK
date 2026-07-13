#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Engine/Renderer/Vulkan/Swapchain.h"
#include "Resource.h"

namespace Lgt::Gpu {

struct DrawCommand {
    uint32_t  frameIndex        = 0;
    uint32_t  vertexBufferIndex = 0;
    uint32_t  indexBufferIndex  = 0;
    uint32_t  materialIndex     = 0;
    glm::mat4 transform         = glm::mat4(1.0f);
};

struct DrawList {
    DrawCommand* commands    = nullptr;
    uint32_t*    indexCounts = nullptr;
    uint32_t     count       = 0;
};

struct FrameUBO {
    glm::vec3 color;
};

class Renderer {
public:
    void Init(GLFWwindow* window);
    void ShutDown();
    void Render(DrawList* list, uint32_t frameIndex);
    void BeginFrame(uint32_t frameIndex);
    void EndFrame();

private:
    GLFWwindow* window_ = nullptr;

    VulkanSwapchain swapchain_;

    // Frame resources
    VkCommandPool                commandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers_;
    std::vector<BufferHandle>    frameUBO_;

    // Sync
    std::vector<VkSemaphore> imageAvailableSems_;
    std::vector<VkSemaphore> renderFinishedSems_;
    std::vector<VkFence>     inFlightFences_;

    uint32_t currentFrame_       = 0;
    uint32_t currentImageIndex_  = 0;
    bool     framebufferResized_ = false;

    VkPipeline   TraingleGfxPipeline_ = VK_NULL_HANDLE;
    BufferHandle vertSSBO_;
    uint32_t     vertGpuIndex = 0;

    void        createCommandPool();
    void        createCommandBuffers();
    void        createSyncObjects();
    void        createUBOS();
    void        recreateSwapchain();
    static void framebufferResizeCallback(GLFWwindow* w, int, int);

    // testing purpose only
    void createTestResources();
};
} // namespace Lgt::Gpu