#pragma once
#include <volk.h>

struct GLFWwindow;

namespace Lgt {

class ImGuiLayer {
public:
    void Init(GLFWwindow* window, VkFormat colorFormat);
    void BeginFrame();
    void EndFrame(VkCommandBuffer cmd);
    void Shutdown();

private:
    VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
};

} // namespace Lgt
