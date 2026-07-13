#pragma once
#include <vulkan/vulkan.h>
#include "Core/Logger.h"

#define VK_CHECK(x)                                                                                                              \
    do {                                                                                                                         \
        VkResult err = (x);                                                                                                      \
        if (err != VK_SUCCESS)                                                                                                   \
            LIGHTVK_ERROR("[Vulkan] {} at {}:{}", VkResultToString(err), __FILE__, __LINE__);                                    \
    } while (0)
