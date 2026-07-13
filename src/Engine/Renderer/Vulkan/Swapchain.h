#pragma once
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>

#include "Helpers.h"

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR        capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class VulkanSwapchain {
public:
    // Call once to create; call again after cleanupSwapchain() to recreate.
    void Init(VkPhysicalDevice physical,
              VkDevice         logical,
              VkSurfaceKHR     surface,
              GLFWwindow*      window,
              uint32_t         graphicsFamily,
              uint32_t         presentFamily);

    // Destroys image views + swapchain; keeps the object reusable.
    void Cleanup(VkDevice logical);

    // Convenience: Cleanup then Init (same args kept internally).
    void Recreate(GLFWwindow* window);

    // Accessors
    VkSwapchainKHR                  Handle() const { return swapchain_; }
    VkFormat                        Format() const { return format_; }
    VkExtent2D                      Extent() const { return extent_; }
    const std::vector<VkImage>&     Images() const { return images_; }
    const std::vector<VkImageView>& ImageViews() const { return imageViews_; }
    uint32_t                        ImageCount() const { return static_cast<uint32_t>(images_.size()); }

    static SwapchainSupportDetails QuerySupport(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    VkSwapchainKHR           swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage>     images_;
    std::vector<VkImageView> imageViews_;
    VkFormat                 format_ = VK_FORMAT_UNDEFINED;
    VkExtent2D               extent_{};

    // Cached for Recreate()
    VkPhysicalDevice physical_       = VK_NULL_HANDLE;
    VkDevice         logical_        = VK_NULL_HANDLE;
    VkSurfaceKHR     surface_        = VK_NULL_HANDLE;
    uint32_t         graphicsFamily_ = 0;
    uint32_t         presentFamily_  = 0;

    void createSwapchain(GLFWwindow* window);
    void createImageViews();

    static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    static VkPresentModeKHR   choosePresentMode(const std::vector<VkPresentModeKHR>& modes);
    static VkExtent2D         chooseExtent(const VkSurfaceCapabilitiesKHR& caps, GLFWwindow* window);
};