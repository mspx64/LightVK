#pragma once
#include "Device.h"
#include "Instance.h"
#include "Allocator.h"
#include "Surface.h"
#include "Uploader.h"

namespace Lgt::Vulkan {

struct Contex {

    VulkanSurface*                 surface   = nullptr;
    VulkanInstance*                instance  = nullptr;
    VulkanDevice*                  device    = nullptr;
    VulkanAllocator*               allocator = nullptr;
    VulkanLoadTimeStagingUploader* uploader  = nullptr;

    void                           Init(GLFWwindow* window);
    void                           Shoutdown();
};

extern Contex g_Contex;

} // namespace Lgt::Vulkan
