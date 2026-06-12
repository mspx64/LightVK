#include <stdexcept>
#include "Surface.h"

void VulkanSurface::Init(VkInstance instance, GLFWwindow* window) {
    instance_ = instance;
    if (glfwCreateWindowSurface(instance_, window, nullptr, &surface_) != VK_SUCCESS)
        throw std::runtime_error("glfwCreateWindowSurface failed");
}

void VulkanSurface::ShutDown() {
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
}