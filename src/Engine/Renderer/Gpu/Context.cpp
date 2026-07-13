#include "Context.h"
#include "Engine/Renderer/Vulkan/Context.h"

namespace Lgt::Gpu {

Context                              g_Context;
ResourcePool<Texture, TextureHandle> g_Textures;
ResourcePool<Buffer, BufferHandle>   g_Buffers;

void Context::Init(GLFWwindow* window) {
    resourceHeap =
        new DescriptorHeap(MAX_RESOURCES * 32 + Vulkan::g_Context.device->DescriptorHeapProperties().minResourceHeapReservedRange);
    samplerHeap =
        new DescriptorHeap(MAX_SAMPLERS * 16 + Vulkan::g_Context.device->DescriptorHeapProperties().minSamplerHeapReservedRange);
    renderer = new Renderer();
    renderer->Init(window);
}

void Context::Shutdown() {
    vkDeviceWaitIdle(Vulkan::g_Context.device->Logical());

    g_Buffers.ForEach([&](Gpu::Buffer& buffer) {
        if (buffer.mapped)
            Vulkan::g_Context.allocator->unmap(buffer.allocation);

        Vulkan::g_Context.allocator->destroyBuffer(buffer.buffer, buffer.allocation);
    });

    g_Buffers.Clear();

    delete resourceHeap;
    delete samplerHeap;

    renderer->ShutDown();
}

} // namespace Lgt::Gpu