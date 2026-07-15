#pragma once
#include <memory>

#include "Engine/Core/Timer.h"
#include "Engine/Scene/World.h"
#include "Engine/Core/InputManager.h"

struct GLFWwindow;

namespace Lgt {

class ImGuiLayer;

class Application {
public:
    Application();
    virtual ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Init();
    void Run();
    void Shutdown();
protected:
    void BeginUi();
    void EndUi();

    virtual void OnInit() {}
    virtual void OnUpdate(uint32_t currentFrame) {}
    virtual void OnShutdown() {}

    GLFWwindow*                   window_ = nullptr;
    std::unique_ptr<World>        world_;
    std::unique_ptr<InputManager> input_;
    std::unique_ptr<Timer>        timer_;
    std::unique_ptr<ImGuiLayer>   imguiLayer_;
};

} // namespace Lgt
