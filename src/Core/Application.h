#pragma once
#include <memory>

#include "Timer.h"
#include "World.h"
#include "InputManager.h"

struct GLFWwindow;

namespace Lgt {

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
    virtual void OnInit() {}
    virtual void OnUpdate(uint32_t currentFrame) {}
    virtual void OnShutdown() {}

    GLFWwindow*                   window_ = nullptr;
    std::unique_ptr<World>        world_;
    std::unique_ptr<InputManager> input_;
    std::unique_ptr<Timer>        timer_;
};

} // namespace Lgt
