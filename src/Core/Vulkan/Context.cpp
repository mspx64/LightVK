#include "Context.h"
#include "Core/Logger.h"

namespace Lgt::Vulkan {
Context g_Context;
void   Context::Init(GLFWwindow* window) {
    VK_CHECK(volkInitialize());

    // TODO modify this to use the RAII
    instance = new VulkanInstance();
    surface  = new VulkanSurface();

    // create Vulkan Instance and load it using volk before creating the device
    instance->Init(ENABLE_VALIDATION, VALIDATION_LAYERS);
    volkLoadInstance(instance->Handle());

    // create surface
    surface->Init(instance->Handle(), window);

    // create vulkan device after the loding instance with volk
    device = new VulkanDevice(instance->Handle(), surface->Handle(), DEVICE_EXTENSIONS, VALIDATION_LAYERS);
    volkLoadDevice(device->Logical());

    allocator = new VulkanAllocator(instance->Handle(), device->Physical(), device->Logical());

    uploader = new VulkanLoadTimeStagingUploader();
}
void Context::Shutdown() {
    vkDeviceWaitIdle(g_Context.device->Logical());
    
    delete uploader;
    delete allocator;
    delete device;

    surface->ShutDown();
    instance->ShutDown();
}

} // namespace Lgt::Vulkan
