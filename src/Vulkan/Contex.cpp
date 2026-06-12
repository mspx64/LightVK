#include "Contex.h"
#include "Core/Logger.h"

namespace Lgt::Vulkan {
Contex g_Contex;
void   Contex::Init(GLFWwindow* window) {
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
    volkLoadDevice(device->logical());

    allocator = new VulkanAllocator(instance->Handle(), device->physical(), device->logical());

    uploader = new VulkanLoadTimeStagingUploader();
}
void Contex::Shoutdown() {
    vkDeviceWaitIdle(g_Contex.device->logical());
    
    delete uploader;
    delete allocator;
    delete device;

    surface->ShutDown();
    instance->ShutDown();
}

} // namespace Lgt::Vulkan
