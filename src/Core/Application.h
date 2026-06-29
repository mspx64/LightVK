#pragma once
#include "Gpu/Renderer.h"
#include "Gpu/Resource.h"

#include "Timer.h"
#include "Logger.h"
#include "World.h"

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

    GLFWwindow* window_ = nullptr;
    World       world_;
    Timer       timer_;
};
} // namespace Lgt
