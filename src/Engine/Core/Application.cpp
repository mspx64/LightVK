#include "Engine/Core/Application.h"
#include "Engine/Core/Timer.h"
#include "Engine/Scene/World.h"
#include "Engine/Core/InputManager.h"
#include "Engine/Core/Logger.h"
#include <GLFW/glfw3.h>
#include "Engine/Renderer/Vulkan/Helpers.h"
#include "Engine/Renderer/Vulkan/Context.h"
#include "Engine/Renderer/Gpu/Context.h"
#include "Engine/UI/ImGuiLayer.h"
#include "Math.h"

namespace Lgt {

void Application::Init() {

    LOG_INIT();

    LIGHTVK_CRITICAL("Testing Critical");
    LIGHTVK_ERROR("Testing Error");
    LIGHTVK_TRACE("Testing Trace");
    LIGHTVK_WARN("Testing Warn");
    LIGHTVK_INFO("Testing Info");

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(WIDTH, HEIGHT, "DoggEngine", nullptr, nullptr);

    Vulkan::g_Context.Init(window_);
    Gpu::g_Context.Init(window_);

    timer_ = std::make_unique<Timer>();
    world_ = std::make_unique<World>();
    input_ = std::make_unique<InputManager>(window_);

    imguiLayer_ = std::make_unique<ImGuiLayer>();
    imguiLayer_->Init(window_, Gpu::g_Context.renderer->SwapchainFormat());

    OnInit();
}

void Application::Run() {
    uint32_t currentFrame = 0;

    while (!glfwWindowShouldClose(window_)) {

        timer_->Tick();
        input_->ResetFrame();
        world_->Update(1.0f);

        OnUpdate(currentFrame);

        glfwPollEvents();
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Application::Shutdown() {
    OnShutdown();
    imguiLayer_->Shutdown();
    Gpu::g_Context.Shutdown();
    Vulkan::g_Context.Shutdown();
}

void Application::BeginUi() {
    auto uiCmd = Gpu::g_Context.renderer->GetUICommandBuffer();
    imguiLayer_->BeginFrame();
    Gpu::g_Context.renderer->BeginRendering(uiCmd, false);
}

void Application::EndUi() {
    auto uiCmd = Gpu::g_Context.renderer->GetUICommandBuffer();
    imguiLayer_->EndFrame(uiCmd);
    Gpu::g_Context.renderer->EndRendering(uiCmd);
}

Application::Application()  = default;
Application::~Application() = default;

} // namespace Lgt
