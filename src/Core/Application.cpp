#include "Application.h"
#include "Vulkan/Contex.h"
#include "Gpu/Contex.h"

#include "Math.h"

namespace Lgt {

void Application::Init() {

    LOG_INIT();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(WIDTH, HEIGHT, "DoggEngine", nullptr, nullptr);

    Vulkan::g_Contex.Init(window_);
    auto props = Vulkan::g_Contex.device->DescriptorHeapProperties();
    LIGHTVK_INFO(
        "bufferDescriptorSize={}, bufferDescriptorAlignment={}", props.bufferDescriptorSize, props.bufferDescriptorAlignment);

    Gpu::g_Contex.Init(window_);

    auto prespective = Matrix::Perspective(1.45, WIDTH / HEIGHT, 1, 100);
    prespective.print();

    OnInit();
}

void Application::Run() {
    uint32_t currentFrame = 0;

    while (!glfwWindowShouldClose(window_)) {
        OnUpdate(currentFrame);
        glfwPollEvents();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Application::Shutdown() {
    OnShutdown();
    Gpu::g_Contex.Shoutdown();
    Vulkan::g_Contex.Shoutdown();
}

} // namespace Lgt
