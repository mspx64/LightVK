#pragma once
#include "Gpu/Renderer.h"
#include "Gpu/Resource.h"

#include "Timer.h"
#include "Logger.h"
#include "World.h"
#include "InputManager.h"

namespace Lgt {

class Application {
public:
    void Init();
    void Run();
    void Shutdown();

    virtual ~Application() = default;

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
