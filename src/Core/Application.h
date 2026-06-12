#pragma once
#include "Gpu/Renderer.h"
#include "Gpu/Resource.h"
#include "Core/Assets.h"
#include "Logger.h"
#include "World.h"

namespace Lgt {
class Application {
public:
    void Init();
    void Run();
    void Shutdown();

private:
    GLFWwindow* window_ = nullptr;
    World       world_;

    // debug purpose only
    Gpu::DrawList BuildDrawList(const Assets::Model& model);
};
} // namespace Lgt
