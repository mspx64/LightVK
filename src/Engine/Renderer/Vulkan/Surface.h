#pragma once

#include "Helpers.h"
#include <GLFW/glfw3.h>

class VulkanSurface {
public:
    void Init(VkInstance instance, GLFWwindow* window);
    void ShutDown();

    VkSurfaceKHR Handle() const { return surface_; }

private:
    VkInstance   instance_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_  = VK_NULL_HANDLE;
};