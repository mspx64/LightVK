#pragma once
#include "../Core.h"
#include "Device.h"
#include "Instance.h"
#include "Allocator.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Uploader.h"

namespace Lgt::Vulkan {

struct Context {

    VulkanSurface*                 surface   = nullptr;
    VulkanInstance*                instance  = nullptr;
    VulkanDevice*                  device    = nullptr;
    VulkanAllocator*               allocator = nullptr;
    VulkanLoadTimeStagingUploader* uploader  = nullptr;

    void                           Init(GLFWwindow* window);
    void                           Shutdown();
};

extern LIGHTVK_API Context g_Context;

} // namespace Lgt::Vulkan
