
#include <stdexcept>
#include <array>
#include "Engine/Core/Logger.h"
#include "Context.h"
#include "Engine/Renderer/Vulkan/Context.h"

#include "Renderer.h"

namespace Lgt::Gpu {
void Renderer::Init(GLFWwindow* window) {

    window_ = window;
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);

    swapchain_.Init(Lgt::Vulkan::g_Context.device->Physical(),
                    Lgt::Vulkan::g_Context.device->Logical(),
                    Lgt::Vulkan::g_Context.surface->Handle(),
                    window_,
                    Lgt::Vulkan::g_Context.device->GraphicsFamily(),
                    Lgt::Vulkan::g_Context.device->PresentFamily());

    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
    createUBOS();

    createTestResources();
}

void Renderer::ShutDown() {
    vkDeviceWaitIdle(Lgt::Vulkan::g_Context.device->Logical());

    swapchain_.Cleanup(Lgt::Vulkan::g_Context.device->Logical());
    vkDestroyPipeline(Lgt::Vulkan::g_Context.device->Logical(), TraingleGfxPipeline_, nullptr);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroySemaphore(Lgt::Vulkan::g_Context.device->Logical(), imageAvailableSems_[i], nullptr);
        vkDestroySemaphore(Lgt::Vulkan::g_Context.device->Logical(), renderFinishedSems_[i], nullptr);
        vkDestroyFence(Lgt::Vulkan::g_Context.device->Logical(), inFlightFences_[i], nullptr);
    }
    vkDestroyCommandPool(Lgt::Vulkan::g_Context.device->Logical(), commandPool_, nullptr);
}

void Renderer::recreateSwapchain() {
    int w = 0, h = 0;
    glfwGetFramebufferSize(window_, &w, &h);
    while (w == 0 || h == 0) {
        glfwGetFramebufferSize(window_, &w, &h);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(Lgt::Vulkan::g_Context.device->Logical());
    swapchain_.Recreate(window_);
}

void Renderer::Render(DrawList* list, uint32_t frameIndex) {

    BeginFrame(frameIndex);
    LGT_ASSERT(list);

    auto cmd = commandBuffers_[currentFrame_];

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(swapchain_.Extent().width);  // Or your window width
    viewport.height   = static_cast<float>(swapchain_.Extent().height); // Or your window height
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain_.Extent();

    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, TraingleGfxPipeline_);

    // TODO-------------------------------------------
    auto* ubo = g_Buffers.Get(frameUBO_[currentFrame_]);
    LGT_ASSERT(ubo);
    FrameUBO ubodata{glm::vec3{1.0, 1.0, 1.0}};
    memcpy(ubo->mapped, &ubodata, sizeof(FrameUBO));
    //------------------------------------------------

    for (int i = 0; i < list->count; ++i) {

        list->commands[i].frameIndex = currentFrame_;

        VkHostAddressRangeConstEXT cpuPushDataInfo{};
        cpuPushDataInfo.size    = sizeof(DrawCommand);
        cpuPushDataInfo.address = &list->commands[i];

        VkPushDataInfoEXT pushDataInfo{};
        pushDataInfo.sType  = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT;
        pushDataInfo.offset = 0;
        pushDataInfo.data   = cpuPushDataInfo;

        vkCmdPushDataEXT(cmd, &pushDataInfo);
        vkCmdDraw(cmd, list->indexCounts[i], 1, 0, 0);
    }

    EndFrame();
}

void Renderer::BeginFrame(uint32_t frameindex) {
    currentFrame_ = frameindex;

    vkWaitForFences(Vulkan::g_Context.device->Logical(), 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(Vulkan::g_Context.device->Logical(),
                                            swapchain_.Handle(), // <-- .Handle()
                                            UINT64_MAX,
                                            imageAvailableSems_[currentFrame_],
                                            VK_NULL_HANDLE,
                                            &currentImageIndex_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("vkAcquireNextImageKHR failed");

    vkResetFences(Lgt::Vulkan::g_Context.device->Logical(), 1, &inFlightFences_[currentFrame_]);

    // resource heap bind info
    VkDeviceAddressRangeEXT resourceDeviceAdderRange{};
    resourceDeviceAdderRange.size    = g_Context.resourceHeap->Size();
    resourceDeviceAdderRange.address = g_Context.resourceHeap->BufferAddress();

    VkBindHeapInfoEXT resourceBind{};
    resourceBind.sType     = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT;
    resourceBind.heapRange = resourceDeviceAdderRange;
    resourceBind.reservedRangeOffset =
        g_Context.resourceHeap->Size() - Vulkan::g_Context.device->DescriptorHeapProperties().minResourceHeapReservedRange;
    resourceBind.reservedRangeSize = Lgt::Vulkan::g_Context.device->DescriptorHeapProperties().minResourceHeapReservedRange;

    // smapler heap bind info
    VkDeviceAddressRangeEXT samplerDeviceAdderRange{};
    samplerDeviceAdderRange.size    = g_Context.samplerHeap->Size();
    samplerDeviceAdderRange.address = g_Context.samplerHeap->BufferAddress();

    VkBindHeapInfoEXT samplerBind{};
    samplerBind.sType     = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT;
    samplerBind.heapRange = samplerDeviceAdderRange;
    samplerBind.reservedRangeOffset =
        g_Context.samplerHeap->Size() - Lgt::Vulkan::g_Context.device->DescriptorHeapProperties().minSamplerHeapReservedRange;
    samplerBind.reservedRangeSize = Lgt::Vulkan::g_Context.device->DescriptorHeapProperties().minSamplerHeapReservedRange;

    VkCommandBuffer cmd = commandBuffers_[currentFrame_];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("vkBeginCommandBuffer failed");

    vkCmdBindResourceHeapEXT(cmd, &resourceBind);
    vkCmdBindSamplerHeapEXT(cmd, &samplerBind);

    VkImageMemoryBarrier renderBarrier{};
    renderBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    renderBarrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED; // Safe catch-all for acquired swapchain images
    renderBarrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderBarrier.srcAccessMask                   = 0;
    renderBarrier.dstAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    renderBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    renderBarrier.image                           = swapchain_.Images()[currentImageIndex_];
    renderBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    renderBarrier.subresourceRange.baseMipLevel   = 0;
    renderBarrier.subresourceRange.levelCount     = 1;
    renderBarrier.subresourceRange.baseArrayLayer = 0;
    renderBarrier.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(cmd,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &renderBarrier);

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView        = swapchain_.ImageViews()[currentImageIndex_]; // Your image view getter
    colorAttachment.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Clear to black

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea           = {{0, 0}, swapchain_.Extent()}; // Your swapchain VkExtent2D
    renderingInfo.layerCount           = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments    = &colorAttachment;

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void Renderer::EndFrame() {
    auto cmd = commandBuffers_[currentFrame_];

    vkCmdEndRendering(cmd);

    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED; // Use COLOR_ATTACHMENT_OPTIMAL if you render to it first
    barrier.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask       = 0;
    barrier.dstAccessMask       = 0;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = swapchain_.Images()[currentImageIndex_]; // You need to implement a getter for the current image handle
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(
        cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        throw std::runtime_error("vkEndCommandBuffer failed");

    VkSemaphore          waitSems[]   = {imageAvailableSems_[currentFrame_]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore          signalSems[] = {renderFinishedSems_[currentImageIndex_]};

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSems;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSems;

    if (vkQueueSubmit(Lgt::Vulkan::g_Context.device->GraphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame_]) !=
        VK_SUCCESS)
        throw std::runtime_error("vkQueueSubmit failed");

    VkSwapchainKHR   swapchains[] = {swapchain_.Handle()}; // <-- .Handle()
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSems;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapchains;
    presentInfo.pImageIndices      = &currentImageIndex_;

    auto result = vkQueuePresentKHR(Lgt::Vulkan::g_Context.device->PresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized_) {
        framebufferResized_ = false;
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("vkQueuePresentKHR failed");
    }
}

void Renderer::createTestResources() {

    // pipeline
    auto vertCode = readFile("shaders/shader.vert.spv");
    auto fragCode = readFile("shaders/shader.frag.spv");

    VkShaderModule vertModule = createShaderModule(Lgt::Vulkan::g_Context.device, vertCode);
    VkShaderModule fragModule = createShaderModule(Lgt::Vulkan::g_Context.device, fragCode);

    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertModule;
    vertStage.pName  = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragModule;
    fragStage.pName  = "main";

    VkPipelineShaderStageCreateInfo stages[] = {vertStage, fragStage};

    // Vertex input — positions hardcoded in shader
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth   = 1.0f;
    rasterizer.cullMode    = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace   = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments    = &blendAttachment;

    std::array<VkDynamicState, 2>    dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineCreateFlags2CreateInfo pipelineflags{};
    pipelineflags.sType = VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO;
    pipelineflags.flags = VK_PIPELINE_CREATE_2_DESCRIPTOR_HEAP_BIT_EXT;

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;

    VkFormat colorFormat                  = swapchain_.Format();
    renderingInfo.pColorAttachmentFormats = &colorFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo{};

    // CHAINING
    pipelineInfo.pNext  = &renderingInfo;
    renderingInfo.pNext = &pipelineflags;

    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = stages;
    pipelineInfo.pVertexInputState   = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlend;
    pipelineInfo.pDynamicState       = &dynamicState;

    pipelineInfo.layout     = VK_NULL_HANDLE;
    pipelineInfo.renderPass = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
            Lgt::Vulkan::g_Context.device->Logical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &TraingleGfxPipeline_) !=
        VK_SUCCESS)
        throw std::runtime_error("vkCreateGraphicsPipelines failed");

    vkDestroyShaderModule(Lgt::Vulkan::g_Context.device->Logical(), fragModule, nullptr);
    vkDestroyShaderModule(Lgt::Vulkan::g_Context.device->Logical(), vertModule, nullptr);

    // vertex buffer -- ssbo

    float positions[6] = {0.0, -0.5, 0.5, 0.5, -0.5, 0.5};
    vertSSBO_          = CreateSSBO(sizeof(positions));
    auto* dstgpubuffer = g_Buffers.Get(vertSSBO_);
    vertGpuIndex       = g_Context.resourceHeap->AllocateSSBO(vertSSBO_);

    Buffer srcBuffer{};

    VkBufferCreateInfo bufferci{};
    bufferci.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferci.size        = 24;
    bufferci.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferci.sharingMode = Vulkan::g_Context.device->GraphicsFamily() == Vulkan::g_Context.device->TransferFamily()
                               ? VK_SHARING_MODE_CONCURRENT
                               : VK_SHARING_MODE_EXCLUSIVE;

    uint32_t queuefamilyindices[]  = {Vulkan::g_Context.device->GraphicsFamily(), Vulkan::g_Context.device->TransferFamily()};
    bufferci.queueFamilyIndexCount = 2;
    bufferci.pQueueFamilyIndices   = queuefamilyindices;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    LGT_ASSERT(Vulkan::g_Context.allocator->createBuffer(bufferci, allocInfo, srcBuffer.buffer, srcBuffer.allocation))

    void* ptr = Vulkan::g_Context.allocator->map(srcBuffer.allocation);
    memcpy(ptr, positions, 24);
    Vulkan::g_Context.allocator->unmap(srcBuffer.allocation);

    VkBufferCopy buffercpy{};
    buffercpy.dstOffset = 0;
    buffercpy.srcOffset = 0;
    buffercpy.size      = 24;

    vkWaitForFences(Vulkan::g_Context.device->Logical(), 1, &inFlightFences_[0], VK_TRUE, UINT64_MAX);
    vkResetFences(Lgt::Vulkan::g_Context.device->Logical(), 1, &inFlightFences_[currentFrame_]);

    VkCommandBufferBeginInfo cmdBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(commandBuffers_[0], &cmdBeginInfo);
    vkCmdCopyBuffer(commandBuffers_[0], srcBuffer.buffer, dstgpubuffer->buffer, 1, &buffercpy);
    vkEndCommandBuffer(commandBuffers_[0]);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffers_[0];

    if (vkQueueSubmit(Lgt::Vulkan::g_Context.device->GraphicsQueue(), 1, &submitInfo, inFlightFences_[0]) != VK_SUCCESS)
        throw std::runtime_error("vkQueueSubmit failed");

    vkQueueWaitIdle(Lgt::Vulkan::g_Context.device->GraphicsQueue());
    Vulkan::g_Context.allocator->destroyBuffer(srcBuffer.buffer, srcBuffer.allocation);
}

void Renderer::framebufferResizeCallback(GLFWwindow* w, int /*width*/, int /*height*/) {
    auto* app                = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(w));
    app->framebufferResized_ = true;
}

void Renderer::createCommandPool() {
    VkCommandPoolCreateInfo ci{};
    ci.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    ci.queueFamilyIndex = Lgt::Vulkan::g_Context.device->GraphicsFamily();

    if (vkCreateCommandPool(Lgt::Vulkan::g_Context.device->Logical(), &ci, nullptr, &commandPool_) != VK_SUCCESS)
        throw std::runtime_error("vkCreateCommandPool failed");
}

void Renderer::createCommandBuffers() {
    commandBuffers_.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool_;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

    if (vkAllocateCommandBuffers(Lgt::Vulkan::g_Context.device->Logical(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS)
        throw std::runtime_error("vkAllocateCommandBuffers failed");
}

void Renderer::createSyncObjects() {
    imageAvailableSems_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSems_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semCI{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo     fenceCI{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        if (vkCreateSemaphore(Lgt::Vulkan::g_Context.device->Logical(), &semCI, nullptr, &imageAvailableSems_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(Lgt::Vulkan::g_Context.device->Logical(), &semCI, nullptr, &renderFinishedSems_[i]) != VK_SUCCESS ||
            vkCreateFence(Lgt::Vulkan::g_Context.device->Logical(), &fenceCI, nullptr, &inFlightFences_[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create sync objects");
    }
}

void Renderer::createUBOS() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        auto bufferHandle = CreateUBO(sizeof(FrameUBO));
        g_Context.resourceHeap->AllocateUBO(bufferHandle);
        frameUBO_.push_back(std::move(bufferHandle));
    }
}

} // namespace Lgt::Gpu