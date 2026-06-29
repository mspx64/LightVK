#include "Application.h"
#include "Vulkan/Context.h"
#include "Gpu/Context.h"

#include "Math.h"

namespace Lgt {

void Application::Init() {

    LOG_INIT();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(WIDTH, HEIGHT, "DoggEngine", nullptr, nullptr);

    Vulkan::g_Context.Init(window_);
    auto props = Vulkan::g_Context.device->DescriptorHeapProperties();
    LIGHTVK_INFO(
        "bufferDescriptorSize={}, bufferDescriptorAlignment={}", props.bufferDescriptorSize, props.bufferDescriptorAlignment);

    Gpu::g_Context.Init(window_);

    auto prespective = Matrix::Perspective(1.45, WIDTH / HEIGHT, 1, 100);
    prespective.print();

    OnInit();
}

void Application::Run() {
    uint32_t currentFrame = 0;
    
    while (!glfwWindowShouldClose(window_)) {
        timer_.Tick();
        OnUpdate(currentFrame);
        glfwPollEvents();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Application::Shutdown() {
    OnShutdown();
    Gpu::g_Context.Shutdown();
    Vulkan::g_Context.Shutdown();
}

} // namespace Lgt
