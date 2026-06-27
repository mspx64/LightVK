#include "Contex.h"
#include <Core/Vulkan/Contex.h>

namespace Lgt::Gpu {

Context                              g_Contex;
ResourcePool<Texture, TextureHandle> g_Textures;
ResourcePool<Buffer, BufferHandle>   g_Buffers;

void Context::Init(GLFWwindow* window) {
    resourceHeap =
        new DescriptorHeap(MAX_RESOURCES * 32 + Vulkan::g_Contex.device->DescriptorHeapProperties().minResourceHeapReservedRange);
    samplerHeap =
        new DescriptorHeap(MAX_SAMPLERS * 16 + Vulkan::g_Contex.device->DescriptorHeapProperties().minSamplerHeapReservedRange);
    renderer = new Renderer();
    renderer->Init(window);
}

void Context::Shoutdown() {
    vkDeviceWaitIdle(Vulkan::g_Contex.device->logical());

    g_Buffers.ForEach([&](Gpu::Buffer& buffer) {
        if (buffer.mapped)
            Vulkan::g_Contex.allocator->unmap(buffer.allocation);

        Vulkan::g_Contex.allocator->destroyBuffer(buffer.buffer, buffer.allocation);
    });

    g_Buffers.clear();

    delete resourceHeap;
    delete samplerHeap;

    renderer->ShutDown();
}

} // namespace Lgt::Gpu